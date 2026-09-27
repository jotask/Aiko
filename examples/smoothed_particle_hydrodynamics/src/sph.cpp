#include "sph.h"

#include "layers/contexts/asset_context.h"
#include "layers/contexts/scene_context.h"
#include "systems/render_system.h"
#include "systems/system_connector.h"

#include <core/random.h>

#include <array>
#include <chrono>
#include <cmath>

namespace sph
{

    void SPHFluidSimulation::connect(aiko::SystemConnector& systemConnector)
    {
        BIND_SYSTEM_REQUIRED_REF(aiko::RenderSystem, systemConnector, m_renderSystem);
    }

    void SPHFluidSimulation::init()
    {

        // Scene settings
        scene().clearColor() = aiko::BLACK;
        scene().ambientLight().color = aiko::WHITE;
        scene().ambientLight().intensity = 0.15f;

        // Init camera
        aiko::GameObject* camera = Instantiate("Camera");
        aiko::CameraComponent* cameraComponent = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Drag);
        camera->transform().position = { 0.0f, 2.5f, 8.0f };
        cameraComponent->setCameraType(aiko::Camera::CameraType::Orthographic);
        cameraComponent->getCamera().position = camera->transform().position;

        m_playground.init(assets().loadShader("model"));

    }

    void SPHFluidSimulation::update()
    {
        m_playground.update();
    }

    void SPHFluidSimulation::render()
    {
        m_playground.render(renderer(), *m_renderSystem);
    }

}
