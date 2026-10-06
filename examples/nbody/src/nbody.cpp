#include "nbody.h"

#include <components/light_component.h>
#include <types/color.h>
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

        scene().clearColor() = aiko::BLACK;
        scene().ambientLight().color = aiko::WHITE;
        scene().ambientLight().intensity = 0.02f;

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly);
        camera->getCamera().position = { 0.0f, 1.0f, 3.0f };

        aiko::GameObject* go = Instantiate("Simulation");
        auto nbody = go->addComponent<NBodyComponent>();
        nbody->applyStablePreset();

        aiko::GameObject* lightObject = Instantiate("CentralLight");
        lightObject->transform().position = go->transform().position;
        aiko::LightComponent* light = lightObject->addComponent<aiko::LightComponent>();
        light->setPointLight(aiko::WHITE, 20.0f);
        light->intensity = 2.0f;

    }

}

