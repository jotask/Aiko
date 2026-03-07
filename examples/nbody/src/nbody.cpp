#include "nbody.h"

#include "models/game_object.h"
#include "components/camera_component.h"
#include "../../../aiko/renderer/src/models/camera.h"

#include <aiko_includes.h>

namespace nbody
{
    void NBody::init()
    {
        auto camera = app->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Fly);
        camera->transform().position = { 0.0f, 1.0f, 3.0f };
        cam->getCamera().position = camera->transform().position;
    }

}

