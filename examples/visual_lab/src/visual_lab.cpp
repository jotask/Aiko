#include "visual_lab.h"

#include "components/camera_component.h"
#include "components/light_component.h"

#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"

#include "models/game_object.h"
#include "models/mesh_factory.h"

#include <post_process/post_process_pipeline.h>

namespace aiko::lab
{

    namespace
    {
        constexpr float SphereRadius = 0.8f;
        constexpr float SphereSpacing = 2.2f;
        constexpr float MatrixZ = -12.0f;

        constexpr std::array<float, 5> MetallicValues{ 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
        constexpr std::array<float, 5> RoughnessValues{ 0.1f, 0.3f, 0.5f, 0.7f, 0.9f};

    }

    void VisualLab::init()
    {
        scene().clearColor() = RAYWHITE;
        scene().ambientLight().color = WHITE;
        scene().ambientLight().intensity = 0.15f;

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly);
        camera->getCamera().position = { 0.0f, 2.5f, 8.0f };

        initLighting();
        initPbrMaterials();

        // Avoid Bloom masking small differences between materials.
        renderer().postProcessing().setEnabled("Bloom", false);

    }

    void VisualLab::update()
    {

    }

    void VisualLab::render()
    {
        renderPbrMatrix();
    }

    void VisualLab::dispose()
    {
        m_sphereMesh.unload();
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

}
