#include "sandbox.h"

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"

#include "particle_component.hpp"

namespace sandbox
{
    void Sandbox::init()
    {
        Application::init();
        m_go = this->createGameObject("SandBox");
        m_go->addComponent<sandbox::ParticlesComponent>();
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

