#include "nbody.h"

#include <models/game_object.h>
#include <components/camera_component.h>
#include <models/camera.h>

#include <aiko_includes.h>

#include "nbody_component.h"
#include "nbody_system.h"

namespace nbody
{

    void NBody::registerSystems()
    {
        app->m_aiko->registerSystem<NBodySystem>();
    }

    void NBody::init()
    {
        auto camera = app->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Fly);
        camera->transform().position = { 0.0f, 1.0f, 3.0f };
        cam->getCamera().position = camera->transform().position;

        aiko::GameObject* go = app->Instantiate("Simulation");
        go->transform().position = { .0f, 0.0f, 0.0f };
        go->transform().rotation = { 0.0f, 0.0f, 0.0f };
        go->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto nbody = go->addComponent<NBodyComponent>();
        nbody->applyStressTestPreset();

    }

}

