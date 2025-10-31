#include "nbody.h"

#include <cmath>

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "models/mesh.h"
#include "components/texture_component.h"
#include "components/pbo_texture_component.h"
#include "components/model_component.h"
#include "models/camera.h"
#include "types/color.h"

#include <aiko_includes.h>

#define TEST_LOAD_MESHES
#define TEST_COMPONENTS
#define TEST_PRIMITVES

namespace nbody
{
    void NBody::init()
    {
        Application::init();

        auto camera = this->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Fly);
        camera->transform()->position = { 0.0f, 1.0f, 3.0f };
        cam->getCamera()->position = camera->transform()->position;

        auto root = Instantiate("Root");

    }

    void NBody::update()
    {
        Application::update();
    }

    void NBody::render()
    {
        Application::render();
    }
}

