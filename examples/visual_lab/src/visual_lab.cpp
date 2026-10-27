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
            FullPBR,
            ColorSpace,
            MaterialColor,
            ShadowValidation,
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
        initColorSpaceTest();
        initMaterialColorTest();
        initShadowValidation();
        initShadowValidationLights();

        // Avoid Bloom masking small differences between materials.
        renderer().postProcessing().setEnabled("Bloom", false);

    }

    void VisualLab::update()
    {

    }

    void VisualLab::render()
    {
        renderImgui();
        syncVisualLabLighting();

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

            case VisualTest::ColorSpace:
                renderColorSpaceTest();
                break;

            case VisualTest::MaterialColor:
                renderMaterialColorTest();
                break;

            case VisualTest::ShadowValidation:
                renderShadowValidation();
                break;

        }
    }

    void VisualLab::dispose()
    {
        m_sphereMesh.unload();
        m_fireHydrantMesh.unload();
        m_colorSpaceQuad.unload();
        m_shadowCubeMesh.unload();
        m_shadowGroundMesh.unload();
    }

    void VisualLab::initLighting()
    {
        GameObject* lightObject = Instantiate("PBR Reference Light");

        LightComponent* light = lightObject->addComponent<LightComponent>();
        m_referenceLight = light;

        light->setDirectional(WHITE, vec3{-0.5f, -0.7f, -1.0f}, 100.0f);

        light->intensity = 3.0f;

    }

    void VisualLab::initPbrMaterials()
    {
        const AssetId pbrShader = assets().loadShader("pbr");

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

        const auto result = assets().importMesh("models/FireHydrantMesh.obj");
        AIKO_ASSERT(static_cast<bool>(result), "Failed to import Fire Hydrant mesh");

        m_fireHydrantMesh.upload(*result);
        AIKO_ASSERT(m_fireHydrantMesh.isValid(), "Fire Hydrant mesh upload failed");

    }

    void VisualLab::initColorSpaceTest()
    {
        constexpr uint Size = 4;

        TextureAsset grayTexture = texture::factory::generateBlank(Size, Size, Color::fromBytes(128, 128, 128, 255));

        const AssetId grayTextureId = assets().createTexture(grayTexture);

        m_colorSpaceQuad.upload(mesh::factory::generateQuad());

        const AssetId shader = assets().loadShader("color_space_validation.vert","color_space_validation.frag");

        m_colorSpaceMaterial.m_shaderId = shader;
        m_colorSpaceMaterial.m_lit = false;
        m_colorSpaceMaterial.m_useVertexColor = false;
        m_colorSpaceMaterial.m_renderState.depthTest = false;
        m_colorSpaceMaterial.m_renderState.depthWrite = false;

        // Same asset ID, sRGB GPU variant.
        m_colorSpaceMaterial.setAlbedoMap(grayTextureId);

        // Same asset ID, linear GPU variant.
        m_colorSpaceMaterial.setTexture("u_secondaryTexture", grayTextureId);
        m_colorSpaceMaterial.setFloat("u_expectedLinear", 128.0f / 255.0f);
        m_colorSpaceMaterial.setFloat("u_expectedSrgb", 0.215861f);
    }

    void VisualLab::initMaterialColorTest()
    {
        constexpr uint Size = 4;

        const Color gray = Color::fromBytes(128, 128, 128, 255);

        TextureAsset grayTexture = texture::factory::generateBlank( Size, Size, gray);

        const AssetId textureId = assets().createTexture(grayTexture);

        const AssetId shaderId = assets().loadShader("multi_texture_validation.vert", "material_color_validation.frag");

        m_materialColorMaterial.m_shaderId = shaderId;
        m_materialColorMaterial.m_baseColor = gray;
        m_materialColorMaterial.m_lit = false;
        m_materialColorMaterial.m_useVertexColor = false;

        m_materialColorMaterial.m_renderState.depthTest = false;
        m_materialColorMaterial.m_renderState.depthWrite = false;

        // Force the hardware sRGB sampling variant.
        m_materialColorMaterial.setAlbedoMap(textureId);
    }

    void VisualLab::initShadowValidation()
    {
        m_shadowCubeMesh.upload(mesh::factory::generateCube());
        m_shadowGroundMesh.upload(mesh::factory::generateMeshPlane(1.0f, 1.0f, 2, 2));

        AIKO_ASSERT(m_shadowCubeMesh.isValid(), "Shadow validation cube is invalid");
        AIKO_ASSERT(m_shadowGroundMesh.isValid(), "Shadow validation ground is invalid");

        const AssetId pbrShader = assets().loadShader("pbr");

        m_shadowCasterMaterial.m_shaderId = pbrShader;
        m_shadowCasterMaterial.m_lit = true;
        m_shadowCasterMaterial.m_useVertexColor = false;
        m_shadowCasterMaterial.m_baseColor = Color{0.9f, 0.4f, 0.15f, 1.0f};
        m_shadowCasterMaterial.setMetallic(0.0f);
        m_shadowCasterMaterial.setRoughness(0.6f);

        m_shadowGroundMaterial = m_shadowCasterMaterial;
        m_shadowGroundMaterial.m_baseColor = Color{0.65f, 0.7f, 0.75f, 1.0f};
        m_shadowGroundMaterial.setRoughness(0.9f);
    }

    void VisualLab::initShadowValidationLights()
    {
        // Directional
        {
            GameObject* object = Instantiate("Shadow Validation Directional");
            LightComponent* light = object->addComponent<LightComponent>();

            m_shadowDirLight.name = "Directional";
            m_shadowDirLight.type = LightType::Directional;
            m_shadowDirLight.object = object;
            m_shadowDirLight.component = light;
            m_shadowDirLight.enabled = true;
            m_shadowDirLight.direction = vec3{-0.5f, -0.7f, -1.0f};
            m_shadowDirLight.color = WHITE;
            m_shadowDirLight.intensity = 3.0f;
            m_shadowDirLight.range = 100.0f;

            light->intensity = 0.0f;
        }

        // Spot A
        {
            GameObject* object = Instantiate("Shadow Validation Spot A");
            LightComponent* light = object->addComponent<LightComponent>();

            m_shadowSpotA.name = "Spot A";
            m_shadowSpotA.type = LightType::Spot;
            m_shadowSpotA.object = object;
            m_shadowSpotA.component = light;
            m_shadowSpotA.enabled = false;
            m_shadowSpotA.position = vec3{0.0f, 8.0f, -12.0f};
            m_shadowSpotA.direction = vec3{0.0f, -1.0f, 0.0f};
            m_shadowSpotA.color = WHITE;
            m_shadowSpotA.intensity = 6.0f;
            m_shadowSpotA.range = 25.0f;
            m_shadowSpotA.innerCos = 0.95f;
            m_shadowSpotA.outerCos = 0.65f;

            object->transform().position = m_shadowSpotA.position;
            light->intensity = 0.0f;
        }

        // Spot B
        {
            GameObject* object = Instantiate("Shadow Validation Spot B");
            LightComponent* light = object->addComponent<LightComponent>();

            m_shadowSpotB.name = "Spot B";
            m_shadowSpotB.type = LightType::Spot;
            m_shadowSpotB.object = object;
            m_shadowSpotB.component = light;
            m_shadowSpotB.enabled = false;
            m_shadowSpotB.position = vec3{6.0f, 6.0f, -10.0f};
            m_shadowSpotB.direction = vec3{-0.8f, -0.6f, -0.2f};
            m_shadowSpotB.color = Color{0.6f, 0.8f, 1.0f, 1.0f};
            m_shadowSpotB.intensity = 5.0f;
            m_shadowSpotB.range = 20.0f;
            m_shadowSpotB.innerCos = 0.92f;
            m_shadowSpotB.outerCos = 0.70f;

            object->transform().position = m_shadowSpotB.position;
            light->intensity = 0.0f;
        }

        // Point A
        {
            GameObject* object = Instantiate("Shadow Validation Point A");
            LightComponent* light = object->addComponent<LightComponent>();

            m_shadowPointA.name = "Point A";
            m_shadowPointA.type = LightType::Point;
            m_shadowPointA.object = object;
            m_shadowPointA.component = light;
            m_shadowPointA.enabled = false;
            m_shadowPointA.position = vec3{3.0f, 3.0f, -10.0f};
            m_shadowPointA.color = Color{1.0f, 0.85f, 0.65f, 1.0f};
            m_shadowPointA.intensity = 8.0f;
            m_shadowPointA.range = 18.0f;

            object->transform().position = m_shadowPointA.position;
            light->intensity = 0.0f;
        }
    }

    void VisualLab::syncVisualLabLighting()
    {
        const bool shadowMode = ActiveTest == VisualTest::ShadowValidation;

        if (m_referenceLight != nullptr)
        {
            m_referenceLight->intensity = shadowMode ? 0.0f : 3.0f;
        }

        if (!shadowMode)
        {
            if (m_shadowDirLight.component != nullptr)  m_shadowDirLight.component->intensity = 0.0f;
            if (m_shadowSpotA.component != nullptr)     m_shadowSpotA.component->intensity = 0.0f;
            if (m_shadowSpotB.component != nullptr)     m_shadowSpotB.component->intensity = 0.0f;
            if (m_shadowPointA.component != nullptr)    m_shadowPointA.component->intensity = 0.0f;
            return;
        }

        // Keep spotlight cone values sane.
        auto fixSpot = [](ValidationLight& light)
        {
            light.innerCos = std::clamp(light.innerCos, 0.01f, 0.999f);
            light.outerCos = std::clamp(light.outerCos, 0.01f, 0.999f);

            if (light.innerCos < light.outerCos)
            {
                std::swap(light.innerCos, light.outerCos);
            }

            if (math::length(light.direction) < 0.0001f)
            {
                light.direction = vec3(0.0f, -1.0f, 0.0f);
            }
        };

        fixSpot(m_shadowSpotA);
        fixSpot(m_shadowSpotB);

        applyValidationLight(m_shadowDirLight);
        applyValidationLight(m_shadowSpotA);
        applyValidationLight(m_shadowSpotB);
        applyValidationLight(m_shadowPointA);
    }

    void VisualLab::applyValidationLight(ValidationLight& light)
    {
        AIKO_ASSERT(light.object != nullptr, "Validation light object is null");
        AIKO_ASSERT(light.component != nullptr, "Validation light component is null");

        light.object->transform().position = light.position;

        switch (light.type)
        {
            case LightType::Directional:
                light.component->setDirectional(light.color, light.direction, light.range);
                break;

            case LightType::Spot:
                light.component->setSpot(light.color, light.direction, light.innerCos, light.outerCos);
                light.component->range = light.range;
                break;

            case LightType::Point:
                light.component->setPointLight(light.color, light.range);
                break;
        }

        light.component->castsShadows = light.castsShadows;
        light.component->intensity = light.enabled ? light.intensity : 0.0f;
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


        if (ActiveTest == VisualTest::ShadowValidation)
        {
            ImGui::SeparatorText("Shadow Validation");

            ImGui::Checkbox("Animate cube", &m_shadowAnimate);
            ImGui::Checkbox("Show ground", &m_shadowShowGround);
            ImGui::Checkbox("Show sphere", &m_shadowShowSphere);

            ImGui::SliderFloat("Cube height", &m_shadowHeight, 0.5f, 5.0f);

            if (!m_shadowAnimate)
            {
                ImGui::SliderFloat("Cube rotation", &m_shadowRotation, 0.0f, 360.0f);
            }

            ImGui::Separator();
            ImGui::SeparatorText("Validation Lights");

            renderValidationLightControls(m_shadowDirLight);
            renderValidationLightControls(m_shadowSpotA);
            renderValidationLightControls(m_shadowSpotB);
            renderValidationLightControls(m_shadowPointA);

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

    void VisualLab::renderColorSpaceTest()
    {
        Transform transform{};

        transform.position = {0.0f, 0.0f, -12.0f};
        transform.scale = {10.0f, 5.0f, 1.0f};

        renderer().drawMesh(transform, m_colorSpaceQuad, m_colorSpaceMaterial);
    }

    void VisualLab::renderMaterialColorTest()
    {
        Transform transform{};

        transform.position = {0.0f, 0.0f, -12.0f};
        transform.scale = {10.0f, 5.0f, 1.0f};

        renderer().drawMesh(transform, m_colorSpaceQuad, m_materialColorMaterial);
    }

    void VisualLab::renderShadowValidation()
    {
        constexpr float SceneZ = -12.0f;

        if (m_shadowAnimate)
        {
            m_shadowRotation += ImGui::GetIO().DeltaTime * 35.0f;

            if (m_shadowRotation >= 360.0f)
            {
                m_shadowRotation -= 360.0f;
            }
        }

        // Receiver: large horizontal ground.
        if (m_shadowShowGround)
        {
            Transform ground{};
            ground.position = {0.0f, -0.5f, SceneZ};
            ground.scale = {14.0f, 1.0f, 14.0f};

            renderer().drawMesh(ground, m_shadowGroundMesh, m_shadowGroundMaterial);
        }

        // Caster: elevated rotating cube.
        Transform cube{};
        cube.position = {-1.5f, m_shadowHeight, SceneZ};
        cube.rotation = {0.0f, m_shadowRotation, 0.0f};
        cube.scale = {1.5f, 1.5f, 1.5f};

        renderer().drawMesh(cube, m_shadowCubeMesh, m_shadowCasterMaterial);

        // Additional rounded caster for silhouette comparison.
        if (m_shadowShowSphere)
        {
            Transform sphere{};
            sphere.position = {2.0f, 1.0f, SceneZ};
            sphere.scale = {1.0f, 1.0f, 1.0f};

            renderer().drawMesh(sphere, m_sphereMesh, m_shadowCasterMaterial);
        }
    }

    void VisualLab::renderValidationLightControls(ValidationLight& light)
    {
        if (!ImGui::TreeNode(light.name))
        {
            return;
        }

        ImGui::Checkbox("Enabled", &light.enabled);
        ImGui::ColorEdit3("Color", &light.color.r);
        ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 10.0f);
        ImGui::Checkbox("Cast Shadows", &light.castsShadows);

        if (light.type == LightType::Spot || light.type == LightType::Point)
        {
            ImGui::DragFloat3("Position", &light.position.x, 0.1f);
            ImGui::SliderFloat("Range", &light.range, 0.5f, 100.0f);
        }

        if (light.type == LightType::Directional || light.type == LightType::Spot)
        {
            ImGui::DragFloat3("Direction", &light.direction.x, 0.01f);
        }

        if (light.type == LightType::Spot)
        {
            ImGui::SliderFloat("Inner Cos", &light.innerCos, 0.01f, 0.999f);
            ImGui::SliderFloat("Outer Cos", &light.outerCos, 0.01f, 0.999f);
        }

        ImGui::TreePop();
    }

}
