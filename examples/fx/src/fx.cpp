#include "fx.h"

#include <cmath>

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "models/mesh.h"
#include "modules/render_primitives.h"
#include "components/texture_component.h"
#include "components/pbo_texture_component.h"
#include "models/camera.h"

#include <aiko_includes.h>

namespace fx
{
    
    void Fx::init()
    {
        Application::init();

        auto camera = this->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Orbit);

        auto root = Instantiate("Root");

    }

    void Fx::update()
    {
        Application::update();
    }

    void Fx::render()
    {
        Application::render();
    }
}

