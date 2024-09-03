#include "sandbox2D.h"

#include <cmath>

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/2d/sprite_component.h"
#include "components/2d/cellular_automaton_component.h"
#include "models/mesh.h"
#include "modules/render_primitives.h"
#include "components/texture_component.h"
#include "components/pbo_texture_component.h"
#include "models/camera.h"

#include <aiko_includes.h>

namespace sandbox
{
    void Sandbox2D::init()
    {
        Application::init();

        auto camera = this->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Drag, aiko::camera::CameraType::Perspective);

        m_sprite = this->Instantiate("CellularAutomaton");
        m_sprite->transform()->position = { 0.0f, -0.0f, 0.0f };
        m_sprite->transform()->rotation = { 0.0f,  0.0f, 0.0f };
        m_sprite->transform()->scale = { 1.0f, 1.0f, 1.0f };
        auto grid = m_sprite->addComponent<aiko::CellularAutomatonComponent>();

    }

    void Sandbox2D::update()
    {
        Application::update();
    }

    void Sandbox2D::render()
    {
        Application::render();
    }
}

