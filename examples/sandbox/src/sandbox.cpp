#include "sandbox.h"

#include <cmath>

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "models/mesh.h"
#include "components/texture_component.h"
#include "components/pbo_texture_component.h"
#include "models/camera.h"
#include "types/color.h"

#include <aiko_includes.h>

// #define TEST_CUBES

namespace sandbox
{
    void Sandbox::init()
    {
        Application::init();

        auto camera = this->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Fly);

        auto root = Instantiate("Root");

#ifdef TEST_CUBES
        m_go1 = this->Instantiate(root, "Cube1");
        m_go1->transform()->position = { 1.0f, 0.0f, 0.0f };
        m_go1->transform()->rotation = { 0.0f, 0.0f, 0.0f };
        m_go1->transform()->scale = { 1.0f, 1.0f, 1.0f };
        auto mesh1 = m_go1->addComponent<aiko::MeshComponent>();
        
        m_go2 = this->Instantiate(root, "Cube2");
        m_go2->transform()->position = { -1.0f, 0.0f, 0.0f };
        m_go2->transform()->rotation = { 0.0f, 0.0f, 0.0f };
        m_go2->transform()->scale = { 1.0f, 1.0f, 1.0f };
        auto mesh2 = m_go2->addComponent<aiko::MeshComponent>();

        m_texture = this->Instantiate(root, "Texture");
        m_texture->transform()->position = { 0.0f, -0.55f, 0.0f };
        m_texture->transform()->rotation = { 0.0f, 0.0f, 0.0f };
        m_texture->transform()->scale = { 1.0f, 1.0f, 1.0f };
        auto mesh3 = m_texture->addComponent<aiko::TextureComponent>();
        
        m_texturePbo = this->Instantiate(root, "PboTexture");
        m_texturePbo->transform()->position = { 0.0f, 0.55f, 0.0f };
        m_texturePbo->transform()->rotation = { 0.0f, 0.0f, 0.0f };
        m_texturePbo->transform()->scale = { 1.0f, 1.0f, 1.0f };
        auto mesh4 = m_texturePbo->addComponent<aiko::PboTextureComponent>();
#endif

    }

    void Sandbox::update()
    {
        Application::update();
#ifdef TEST_CUBES
        static float angle = 0.0f;
        angle += 25.0f * getlDeltaTime();
        angle = fmod(angle, 360.0f);
        m_go1->transform()->rotation = {  angle, 0.0f, 0.0f };
        m_go2->transform()->rotation = { -angle, 0.0f, 0.0f };
#endif
    }

    void Sandbox::render()
    {
        Application::render();

        const aiko::vec3 position = { 0.0f,  0.0f, 0.0f };
        const aiko::vec3 size = { 1.0f, 1.0f, 1.0f };
        // getRenderSystem()->drawRectangle(position, size);

#if true

        static aiko::vec3 circlePosition = { -0.5f,  0.0f, 0.0f };
        static aiko::vec3 trianglePosition = { 0.0f, -0.5f, 0.0f };
        static aiko::vec3 rectanglePosition = { 0.5f, 0.0f, 0.0f };

        constexpr const float SIZE = 1.0f;

        // 2D
        getRenderSystem()->drawRectangle({ 0.0f, 1.0f, 0.0f }, aiko::vec3(SIZE));
        getRenderSystem()->drawPoint({ 1.0f, 1.0f, 0.0f });
        // getRenderSystem()->renderLine(aiko::vec3(-1, 0, 0), aiko::vec3(1, 0, 0));
        // getRenderSystem()->renderCircle(circlePosition, 0.1);
        // getRenderSystem()->drawTriangle(trianglePosition, 0.25f);
        // getRenderSystem()->renderNgon(aiko::vec3(0.5f, 0.0f, 0.0f), 0.25f, 6);

        // 3D
        getRenderSystem()->renderSphere({ -1.0f, 0.0f, 0.0f }, SIZE);
        getRenderSystem()->drawPyramid({ 0.0f, 0.0f, 0.0f }, SIZE, SIZE);
        getRenderSystem()->drawCube({ 1.0f, 0.0f, 0.0f }, aiko::vec3(SIZE));
        getRenderSystem()->renderCylinder({ -2.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 10);

#endif

    }
}

