#include "sandbox.h"

#include <cmath>

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "models/mesh.h"
#include "modules/render_primitives.h"

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

        static aiko::vec3 circlePosition = { -0.5f,  0.0f, 0.0f };
        static aiko::vec3 trianglePosition = { 0.0f, -0.5f, 0.0f };
        static aiko::vec3 rectanglePosition = { 0.5f, 0.0f, 0.0f };

        // 2D
        aiko::Primitives::drawPoint({ 0, 0.5, 0 });
        aiko::Primitives::renderLine(aiko::vec3(-1, 0, 0), aiko::vec3(1, 0, 0));
        aiko::Primitives::renderCircle(circlePosition, 0.1);
        aiko::Primitives::drawTriangle(trianglePosition, 0.25f);
        aiko::Primitives::drawRectangle(rectanglePosition, aiko::vec3(0.5f, 0.5f, 0.5f));
        aiko::Primitives::renderNgon(aiko::vec3(0.5f, 0.0f, 0.0f), 0.25f, 6);

        // 3D
        {
            auto tmp = circlePosition;
            tmp.z = 1.0f;
            aiko::Primitives::renderSphere(tmp, 0.1);
        }
        {
            auto tmp = trianglePosition;
            tmp.z = 1.0f;
            aiko::Primitives::drawPyramid(tmp, 0.25f, 0.25f);
        }
        {
            auto tmp = rectanglePosition;
            tmp.z = 1.0f;
            aiko::Primitives::drawCube(tmp, aiko::vec3(0.5f, 0.5f, 0.5f));
        }
        aiko::Primitives::renderCylinder({ -1.5f, -0.5f, 0.0f }, 0.25f, 0.5f, 10);

    }
}

