#include "sandbox.h"

#include <cmath>

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

        m_go1 = this->createGameObject("Cube1");
        m_go1->transform()->position = { 1.0f, 0.0f, 0.0f };
        m_go1->transform()->rotation = { 0.0f, 0.0f, 0.0f };
        m_go1->transform()->scale = { 1.0f, 1.0f, 1.0f };
        auto mesh1 = m_go1->addComponent<aiko::MeshComponent>();

        m_go2 = this->createGameObject("Cube2");
        m_go2->transform()->position = { -1.0f, 0.0f, 0.0f };
        m_go2->transform()->rotation = { 0.0f, 0.0f, 0.0f };
        m_go2->transform()->scale = { 1.0f, 1.0f, 1.0f };
        auto mesh2 = m_go2->addComponent<aiko::MeshComponent>();
    }

    void Sandbox::update()
    {
        Application::update();
        static float angle = 0.0f;
        angle += 10.0f * getlDeltaTime();
        angle = fmod(angle, 360.0f);
        m_go1->transform()->rotation = { angle, 0.0f, 0.0f };
        m_go2->transform()->rotation = { 0.0f, 0.0f, 0.0f };
    }

    void Sandbox::render()
    {
        Application::render();
    }
}

