#include "sph.h"

#include "layers/contexts/asset_context.h"
#include "layers/contexts/input_context.h"
#include "layers/contexts/scene_context.h"
#include "systems/render_system.h"
#include "systems/asset_system.h"
#include "systems/system_connector.h"

#include <models/frame_buffer.h>
#include <models/texture.h>
#include <math/math_bounds.h>
#include <core/random.h>

#include <array>
#include <chrono>
#include <cmath>

namespace sph
{

    void SPHFluidSimulation::connect(aiko::SystemConnector& systemConnector)
    {
        BIND_SYSTEM_REQUIRED_REF(aiko::RenderSystem, systemConnector, m_renderSystem);
        BIND_SYSTEM_REQUIRED_REF(aiko::AssetSystem, systemConnector, m_assetSystem);
    }

    void SPHFluidSimulation::init()
    {

        // Scene settings
        scene().clearColor() = aiko::BLACK;
        scene().ambientLight().color = aiko::WHITE;
        scene().ambientLight().intensity = 0.15f;

        // Init camera
        aiko::GameObject* camera = Instantiate("Camera");
        m_cameraComponent = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Drag);
        camera->transform().position = { 0.0f, 2.5f, 42.0f };
        m_cameraComponent->setCameraType(aiko::Camera::CameraType::Orthographic);
        m_cameraComponent->getCamera().position = camera->transform().position;

        m_playground.init(assets().loadShader("model"), assets().loadShader("sph/sph_gpuinst.vs", "model.fs"), *m_assetSystem);

    }

    void SPHFluidSimulation::update()
    {

        const aiko::vec2 mouse = input().getMouseFramebufferPosition();

        const aiko::TextureInfo targetInfo = m_renderSystem->getTargetTexture().getColorTexture().getInfo();

        const aiko::ivec2 framebufferSize =
        {
            static_cast<int>(targetInfo.width),
            static_cast<int>(targetInfo.height)
        };

        const aiko::Camera& camera = m_cameraComponent->getCamera();

        const aiko::vec2 viewportPosition =
        {
            mouse.x / static_cast<float>(framebufferSize.x),
            mouse.y / static_cast<float>(framebufferSize.y)
        };

        const aiko::Ray ray = aiko::math::unprojectRay(viewportPosition, camera.getViewMatrix(), camera.getProjectionMatrix(framebufferSize));

        aiko::vec3 mouseWorld = {};

        if (std::fabs(ray.direction.z) > 1e-6f)
        {
            const float t = -ray.origin.z / ray.direction.z;
            mouseWorld = ray.origin + ray.direction * t;
        }

        m_playground.update(input(), mouseWorld);
    }

    void SPHFluidSimulation::render()
    {
        m_playground.render(renderer(), *m_renderSystem);
    }

}
