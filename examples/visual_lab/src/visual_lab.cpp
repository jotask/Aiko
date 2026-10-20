#include "visual_lab.h"

#include "assets/asset_importer.h"

#include "components/camera_component.h"
#include "components/light_component.h"

#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"

#include "models/game_object.h"
#include "models/mesh_factory.h"
#include "models/texture_factory.h"

#include <post_process/post_process_pipeline.h>

#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

#include <cmath>

namespace aiko::lab
{

    namespace
    {
        enum class VisualTest
        {
            MaterialMatrix,
            TextureMaps,
            FullPBR
        };

        constexpr float SphereRadius = 0.8f;
        constexpr float SphereSpacing = 2.2f;
        constexpr float MatrixZ = -12.0f;

        constexpr std::array<float, 5> MetallicValues{ 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
        constexpr std::array<float, 5> RoughnessValues{ 0.1f, 0.3f, 0.5f, 0.7f, 0.9f};

        static VisualTest ActiveTest = VisualTest::TextureMaps;

    }

    void VisualLab::init()
    {
        scene().clearColor() = BLACK;
        scene().ambientLight().color = WHITE;
        scene().ambientLight().intensity = 0.15f;

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly);
        camera->getCamera().position = { 0.0f, 2.5f, 8.0f };

        initLighting();
        initPbrMaterials();
        initTextureTests();
        initFullPbrTest();

        // Avoid Bloom masking small differences between materials.
        renderer().postProcessing().setEnabled("Bloom", false);

    }

    void VisualLab::update()
    {

    }

    void VisualLab::render()
    {
        renderImgui();
        switch (ActiveTest)
        {
            case VisualTest::MaterialMatrix:
                renderPbrMatrix();
                break;

            case VisualTest::TextureMaps:
                renderTextureTests();
                break;

            case VisualTest::FullPBR:
                renderFullPbrTest();
                break;
        }
    }

    void VisualLab::dispose()
    {
        m_sphereMesh.unload();
        m_fireHydrantMesh.unload();
    }

    void VisualLab::initLighting()
    {
        GameObject* lightObject = Instantiate("PBR Reference Light");

        LightComponent* light = lightObject->addComponent<LightComponent>();

        light->setDirectional(
            WHITE,
            vec3{-0.5f, -0.7f, -1.0f},
            100.0f
        );

        light->intensity = 3.0f;
    }

    void VisualLab::initPbrMaterials()
    {
        const AssetId pbrShader = assets().loadShader("pbr");

        // Upload one sphere mesh and reuse it for every material.
        m_sphereMesh.upload(mesh::factory::generateMeshSphere(48, 48));

        AIKO_ASSERT(m_sphereMesh.isValid(), "PBR reference mesh is invalid");

        for (int row = 0; row < GridSize; ++row)
        {
            for (int col = 0; col < GridSize; ++col)
            {
                const int index = row * GridSize + col;

                Material& material = m_pbrMaterials[index];

                material.m_shaderId = pbrShader;
                material.m_lit = true;
                material.m_useVertexColor = false;

                // Keep color constant to isolate BRDF parameters.
                material.m_baseColor = Color{ 0.9f, 0.65f, 0.25f, 1.0f };

                material.setMetallic(MetallicValues[col]);
                material.setRoughness(RoughnessValues[row]);
            }
        }
    }

    void VisualLab::initTextureTests()
    {
        // Start with identical dielectric PBR materials.
        for (Material& material : m_textureTestMaterials)
        {
            material = m_pbrMaterials[2];
            material.m_baseColor = WHITE;
            material.setMetallic(0.0f);
            material.setRoughness(0.4f);
        }

        // 0: Baseline material without texture maps.

        // 1: Albedo texture (sRGB).
        m_textureTestMaterials[1].setAlbedoMap(assets().loadTexture("texel_checker.png"));

        // 2: Packed metallic/roughness texture.
        // R = unused, G = roughness, B = metallic.
        constexpr uint Size = 256;

        TextureAsset mrTexture = texture::factory::generateBlank(Size, Size, WHITE);

        for (uint y = 0; y < Size; ++y)
        {
            for (uint x = 0; x < Size; ++x)
            {
                const float roughness = y < Size / 2 ? 0.2f : 0.85f;
                const float metallic = (x / 32) % 2 == 0 ? 0.0f : 1.0f;
                mrTexture.pixels[y * Size + x] = Color{1.0f, roughness, metallic, 1.0f};
            }
        }

        m_textureTestMaterials[2].setMetallic(1.0f);
        m_textureTestMaterials[2].setRoughness(1.0f);

        m_textureTestMaterials[2].setMetallicRoughnessMap(assets().createTexture(mrTexture));

        // 3: No normal map.

        // 4: Flat tangent-space normal map.
        TextureAsset flatNormal = texture::factory::generateBlank(Size, Size, Color{0.5f, 0.5f, 1.0f, 1.0f});

        m_textureTestMaterials[4].setNormalMap(assets().createTexture(flatNormal));

        // 5: Perturbed tangent-space normal map.
        TextureAsset waveNormal = flatNormal;

        constexpr float TwoPi = 6.28318530718f;

        for (uint y = 0; y < Size; ++y)
        {
            for (uint x = 0; x < Size; ++x)
            {
                const float u = static_cast<float>(x) / Size;
                const float v = static_cast<float>(y) / Size;

                const float nx = 0.65f * std::sin(TwoPi * 8.0f * u);
                const float ny = 0.25f * std::sin(TwoPi * 6.0f * v);
                const float nz = std::sqrt(1.0f - nx * nx - ny * ny);

                waveNormal.pixels[y * Size + x] = Color{nx * 0.5f + 0.5f, ny * 0.5f + 0.5f, nz * 0.5f + 0.5f, 1.0f};
            }
        }

        m_textureTestMaterials[5].setNormalMap(assets().createTexture(waveNormal));

        // 6: Ambient occlusion pattern.
        TextureAsset aoTexture = texture::factory::generateBlank(Size, Size, WHITE);

        for (uint y = 0; y < Size; ++y)
        {
            for (uint x = 0; x < Size; ++x)
            {
                const float ao = ((x / 32 + y / 32) % 2 == 0) ? 1.0f : 0.15f;

                aoTexture.pixels[y * Size + x] = Color{ao, ao, ao, 1.0f};
            }
        }

        m_textureTestMaterials[6].setAmbientOcclusionMap(assets().createTexture(aoTexture));

        // 7: Emissive color.
        m_textureTestMaterials[7].m_baseColor = Color{0.05f, 0.05f, 0.05f, 1.0f};

        m_textureTestMaterials[7].setEmissiveColor(Color{1.0f, 0.15f, 0.02f, 1.0f});

        m_textureTestMaterials[7].setEmissiveStrength(3.0f);

        // 8: Emissive texture.
        m_textureTestMaterials[8] = m_textureTestMaterials[7];

        m_textureTestMaterials[8].setEmissiveMap(assets().loadTexture("texel_checker.png"));
    }

    void VisualLab::initFullPbrTest()
    {
        const AssetId pbrShader = assets().loadShader("pbr");

        // Reuse the same Fire Hydrant assets already tested in RenderLab.
        const AssetId albedo = assets().loadTexture("textures/pbr/fire_hydrant/albedo.png");

        const AssetId normal = assets().loadTexture("textures/pbr/fire_hydrant/normal.png");

        const AssetId orm = assets().loadTexture("textures/pbr/fire_hydrant/orm.png");

        // Base material: complete texture set.
        Material& complete = m_fullPbrMaterials[0];

        complete.m_shaderId = pbrShader;
        complete.m_baseColor = WHITE;
        complete.m_lit = true;
        complete.m_useVertexColor = false;

        complete.setMetallic(1.0f);
        complete.setRoughness(1.0f);

        complete.setAlbedoMap(albedo);
        complete.setNormalMap(normal);
        complete.setMetallicRoughnessMap(orm);
        complete.setAmbientOcclusionMap(orm);

        // Same material without tangent-space normal mapping.
        m_fullPbrMaterials[1] = complete;
        m_fullPbrMaterials[1].clearTexture("u_normalTexture");

        // Same material without ORM.
        m_fullPbrMaterials[2] = complete;
        m_fullPbrMaterials[2].clearTexture("u_metallicRoughnessTexture");
        m_fullPbrMaterials[2].clearTexture("u_occlusionTexture");

        // Explicit neutral properties for the no-ORM comparison.
        m_fullPbrMaterials[2].setMetallic(0.0f);
        m_fullPbrMaterials[2].setRoughness(0.5f);

        const AssetImportResult<MeshAsset> result = AssetImporter::loadMesh("models/FireHydrantMesh.obj");
        AIKO_ASSERT(static_cast<bool>(result), "Failed to import Fire Hydrant mesh");

        m_fireHydrantMesh.upload(*result);
        AIKO_ASSERT(m_fireHydrantMesh.isValid(), "Fire Hydrant mesh upload failed");

    }

    void VisualLab::renderImgui()
    {
        if (ImGui::BeginCombo("VisualMode##Test", magic_enum::enum_name(ActiveTest).data()))
        {
            for (const auto current : magic_enum::enum_values<VisualTest>())
            {
                const bool selected = ActiveTest == current;

                if (ImGui::Selectable(magic_enum::enum_name(current).data(), selected))
                {
                    ActiveTest = current;
                }

                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }

    void VisualLab::renderPbrMatrix()
    {
        const float halfGrid = static_cast<float>(GridSize - 1) * 0.5f;

        for (int row = 0; row < GridSize; ++row)
        {
            for (int col = 0; col < GridSize; ++col)
            {
                const int index = row * GridSize + col;

                Transform transform{};

                transform.position = {
                    (static_cast<float>(col) - halfGrid) * SphereSpacing,
                    (halfGrid - static_cast<float>(row)) * SphereSpacing,
                    MatrixZ
                };

                transform.scale = {
                    SphereRadius,
                    SphereRadius,
                    SphereRadius
                };

                renderer().drawMesh(transform, m_sphereMesh, m_pbrMaterials[index]);
            }
        }
    }

    void VisualLab::renderTextureTests()
    {
        constexpr int Columns = 3;
        constexpr int Rows = 3;

        constexpr float Spacing = 3.0f;
        constexpr float Radius = 1.0f;
        constexpr float Z = -12.0f;

        for (int row = 0; row < Rows; ++row)
        {
            for (int col = 0; col < Columns; ++col)
            {
                const int index = row * Columns + col;

                Transform transform{};

                transform.position = {
                    (static_cast<float>(col) - 1.0f) * Spacing,
                    (1.0f - static_cast<float>(row)) * Spacing,
                    Z
                };

                transform.scale = {Radius, Radius, Radius};

                renderer().drawMesh(transform, m_sphereMesh, m_textureTestMaterials[index]);
            }
        }
    }

    void VisualLab::renderFullPbrTest()
    {
        if (!m_fireHydrantMesh.isValid())
        {
            return;
        }

        constexpr float Spacing = 3.5f;
        constexpr float ModelScale = 1.0f;
        constexpr float Z = -12.0f;

        for (int i = 0; i < FullPbrTestCount; ++i)
        {
            Transform transform{};
            transform.position = { (static_cast<float>(i) - 1.0f) * Spacing, 0.0f, Z};
            transform.scale = { ModelScale, ModelScale, ModelScale };
            renderer().drawMesh(transform, m_fireHydrantMesh, m_fullPbrMaterials[i]);
        }
    }

}
