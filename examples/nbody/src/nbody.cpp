#include "nbody.h"

#include "layers/contexts/scene_context.h"
#include "nbody_component.h"
#include "nbody_system.h"

#include <components/camera_component.h>
#include <models/camera.h>
#include <models/game_object.h>
#include <systems/system_registry.h>

namespace nbody
{

    void NBody::registerSystems(aiko::SystemRegistry& registry)
    {
        registry.add<NBodySystem>();
    }

    void NBody::init()
    {

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly);
        camera->getCamera().position = { 0.0f, 1.0f, 3.0f };

        aiko::GameObject* go = Instantiate("Simulation");
        auto nbody = go->addComponent<NBodyComponent>();
        nbody->applyStressTestPreset();

    }

}

