#include "sandbox.h"

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "models/mesh.h"

namespace sandbox
{
    void Sandbox::init()
    {
        Application::init();
        m_go = this->createGameObject("SandBox");
        auto mesh = m_go->addComponent<aiko::MeshComponent>();
    }

    void Sandbox::update()
    {
        Application::update();
    }

    void Sandbox::render()
    {
        Application::render();
    }
}

