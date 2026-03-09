#include "sandbox.h"

#include <cmath>
#include <chrono>

#include <application/application.h>

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "components/sprite_component.h"
#include "components/model_component.h"
#include "../../../aiko/renderer/src/models/camera.h"
#include "../../../aiko/core/src/types/color.h"

#include <aiko_includes.h>
#include <core/random.h>

#include "components/compute_shader_component.h"

#define TEST_LOAD_MESHES
#define TEST_COMPONENTS
#define TEST_PRIMITIVES
#define TEST_LIGHTS
#define TEST_PARTICLE_CS

namespace sb
{
    void Sandbox::init()
    {

        auto camera = app->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Fly);
		camera->transform().position = { 0.0f, 1.0f, 3.0f };
		cam->getCamera().position = camera->transform().position;

        auto root = app->Instantiate("Root");

#ifdef TEST_LOAD_MESHES

        auto go1 = app->Instantiate(root, "Church");
        go1->transform().position = { 0.0f, 0.0f, -15.0f };
        go1->transform().rotation = { 0.0f, 0.0f, 0.0f };
        go1->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto model1 = go1->addComponent<aiko::ModelComponent>();
        model1->load("church.obj");

        auto go2 = app->Instantiate(root, "Barracks");
        go2->transform().position = { -50.0f, 0.0f, -15.0f };
        go2->transform().rotation = { 0.0f, 0.0f, 0.0f };
        go2->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto model2 = go2->addComponent<aiko::ModelComponent>();
        model2->load("barracks.obj");

        auto go3 = app->Instantiate(root, "Watermill");
        go3->transform().position = { 50.0f, 0.0f, -15.0f };
        go3->transform().rotation = { 0.0f, 0.0f, 0.0f };
        go3->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto model3 = go3->addComponent<aiko::ModelComponent>();
        model3->load("watermill.obj");

        auto go4 = app->Instantiate(root, "Robot");
        go4->transform().position = { 0.0f, 0.0f, 5.0f };
        go4->transform().rotation = { 0.0f, 0.0f, 0.0f };
        const float scale = 0.25f;
        go4->transform().scale = { scale, scale, scale };
        auto model4 = go4->addComponent<aiko::ModelComponent>();
        model4->load("robot.glb");

#endif

#ifdef TEST_COMPONENTS
        m_go1 = app->Instantiate(root, "Cube1");
        m_go1->transform().position = { 1.0f, 0.0f, 5.0f };
        m_go1->transform().rotation = { 0.0f, 0.0f, 0.0f };
        m_go1->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto mesh1 = m_go1->addComponent<aiko::MeshComponent>();
        mesh1->loadDebugCube();
        
        m_go2 = app->Instantiate(root, "Cube2");
        m_go2->transform().position = { -1.0f, 0.0f, 5.0f };
        m_go2->transform().rotation = { 0.0f, 0.0f, 0.0f };
        m_go2->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto mesh2 = m_go2->addComponent<aiko::MeshComponent>();
        mesh2->loadDebugCube();

        m_texture = app->Instantiate(root, "Texture");
        m_texture->transform().position = { 0.0f, -0.55f, 5.0f };
        m_texture->transform().rotation = { 0.0f, 0.0f, 0.0f };
        m_texture->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto mesh3 = m_texture->addComponent<aiko::SpriteComponent>();
        mesh3->load("texel_checker.png");

        m_texturePbo = app->Instantiate(root, "PboTexture");
        m_texturePbo->transform().position = { 0.0f, 0.55f, 5.0f };
        m_texturePbo->transform().rotation = { 0.0f, 0.0f, 0.0f };
        m_texturePbo->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto mesh4 = m_texturePbo->addComponent<aiko::SpriteComponent>();
        mesh4->create(128, 128);

        aiko::MaterialAsset& material = mesh4->getMaterial();
        material.lit = false;
        // material.setTextureFilter(aiko::TextureFilter::Nearest, aiko::TextureFilter::Nearest);
        // material.setTextureMipFilter(aiko::TextureMipFilter::None);
        // material.setTextureWrapMode(aiko::TextureWrapMode::Clamp, aiko::TextureWrapMode::Clamp);

#endif

#ifdef TEST_LIGHTS
        constexpr float radiusSpawn = 10.0f;
        for (size_t i = 0 ; i < 10; ++i )
        {
            auto* obj = app->Instantiate(root, "Light");
            obj->transform().position = {
                aiko::utils::getRandomValue(-radiusSpawn, radiusSpawn),
                aiko::utils::getRandomValue(-radiusSpawn, radiusSpawn),
                aiko::utils::getRandomValue(-radiusSpawn, radiusSpawn),
            };
            auto cmp = obj->addComponent<aiko::LightComponent>();
            cmp->setPointLight(aiko::Color::getRandomColor(), 1.0f);
            const LightInst ints = {
                .obj = obj,
                .cmp = cmp,
                .angle = aiko::utils::getRandomValue(0.0f, 360.f)
            };
            m_lights.push_back(ints);
        }
#endif

#ifdef TEST_PARTICLE_CS
        auto ps = app->Instantiate(root, "Particles");
        auto psCmp = ps->addComponent<aiko::ComputeShaderComponent>();
#endif

    }

    void Sandbox::update()
    {
#ifdef TEST_COMPONENTS
        {
            static float angle = 0.0f;
            angle += 25.0f * app->getlDeltaTime();
            angle = fmod(angle, 360.0f);
            m_go1->transform().rotation = {  angle, 0.0f, 0.0f };
            m_go2->transform().rotation = { -angle, 0.0f, 0.0f };

            {

                aiko::AikoPtr<aiko::SpriteComponent> cmp = m_texturePbo->getComponent<aiko::SpriteComponent>();

                static auto lastTime = std::chrono::steady_clock::now();
                static double accumulatedTime = 0.0;
                static const double interval = 1 / 60.0f;

                auto currentTime = std::chrono::steady_clock::now();
                std::chrono::duration<double> delta = currentTime - lastTime;
                lastTime = currentTime;

                accumulatedTime += delta.count();
                bool should_update = false;

                if (accumulatedTime >= interval)
                {
                    accumulatedTime -= interval; // Handle possible overflow
                    should_update = true;
                }

                if (should_update)
                {

                    struct Particle
                    {
                        aiko::ivec2 pos;
                        aiko::ivec2 dir;
                        aiko::Color col;
                    };

                    constexpr auto N_PARTICLES = 100;
                    constexpr bool S_CLEAR_BRACKGROUND = false;

                    static std::vector<Particle> s_particles;

                    const aiko::MaterialAsset material = cmp->getMaterial();

                    // AIKO_ASSERT(material.m_diffuseTexture.isValid(), "Invalid texture?")
                    // const auto info = material.m_diffuseTexture.getInfo();

                    // TEMP for compilation
                    constexpr int texWidth = 128;
                    constexpr int texHeight = 128;
                    const int w = texWidth - 1;
                    const int h = texHeight - 1;

                    if (s_particles.size() != N_PARTICLES)
                    {
                        s_particles.clear();
                        for (uint i = 0 ; i < N_PARTICLES; i++)
                        {
                            aiko::ivec2 pos = aiko::ivec2(aiko::utils::getRandomValue(0, w), aiko::utils::getRandomValue(0, h));
                            aiko::ivec2 dir = aiko::ivec2(aiko::utils::getRandomValue(-1, 1), aiko::utils::getRandomValue(-1, 1));
                            aiko::Color col = aiko::Color::getRandomColor();
                            s_particles.push_back({pos, dir, col});
                        }
                    }
                    else
                    {
                        for(auto& it : s_particles)
                        {

                            {
                                if (it.pos.x == 0 || it.pos.x == w)
                                {
                                    it.dir.x *= -1;
                                }
                                if (it.pos.y == 0 || it.pos.y == h)
                                {
                                    it.dir.y *= -1;
                                }
                            }

                            it.pos.x += it.dir.x;
                            it.pos.y += it.dir.y;

                            it.pos.x = aiko::math::clamp(it.pos.x, 0, w);
                            it.pos.y = aiko::math::clamp(it.pos.y, 0, h);
                        }
                    }

                    if (S_CLEAR_BRACKGROUND)
                    {
                        std::vector<aiko::Color> pixels;
                        pixels.resize(texWidth * texHeight, aiko::RAYWHITE);
                        cmp->setPixels(std::move(pixels));
                    }
                    for (auto it : s_particles)
                    {
                        cmp->setPixel(it.pos.x, it.pos.y, it.col);
                    }
                    cmp->refresh();

                }

            }

        }
#endif
#ifdef TEST_LIGHTS
        {
            for (auto& light : m_lights)
            {
                light.angle += 1.0f * app->getlDeltaTime();
                aiko::vec3 pos = {std::sin(light.angle), 0.0f, std::cos(light.angle)};
                light.obj->transform().position = pos;
            }
        }
#endif
    }

    void Sandbox::render()
    {

#ifdef TEST_PRIMITIVES

        constexpr const float SIZE = 1.0f;

        // 2D
        app->getRenderSystem()->drawPoint({ 1.0f, 1.0f, 0.0f });
        app->getRenderSystem()->renderLine({ -2.0f, -1.0f, 0.0f }, { 2.0f, -1.0f, 0.0f });

        app->getRenderSystem()->drawRectangle({ 0.0f, 1.0f, 0.0f }, SIZE);
        app->getRenderSystem()->renderCircle({ 2.0f, 1.0f, 0.0f }, SIZE);
        app->getRenderSystem()->drawTriangle({ -1.0f, 1.0f, 0.0f }, SIZE);
        app->getRenderSystem()->renderNgon({ -2.0f, 1.0f, 0.0f }, SIZE, 6);

        // 3D
        app->getRenderSystem()->drawCube({ 1.0f, 0.0f, 0.0f }, SIZE);
        app->getRenderSystem()->drawPyramid({ 0.0f, 0.0f, 0.0f }, SIZE);
        app->getRenderSystem()->renderSphere({ -1.0f, 0.0f, 0.0f }, SIZE);
        app->getRenderSystem()->renderCylinder({ -2.0f, 0.0f, 0.0f }, SIZE, 6);
        app->getRenderSystem()->renderPolygon({ -3.0f, 0.0f, 0.0f }, SIZE, 6, 6);

        app->getRenderSystem()->renderTorus({ 2.0f, 0.0f, 0.0f }, SIZE);
        app->getRenderSystem()->renderKnot({ 3.0f, 0.0f, 0.0f }, SIZE);

        app->getRenderSystem()->drawPlane({ 0.0f, -2.0f, 0.0f }, SIZE);

#endif


#ifdef TEST_LIGHTS
        {
            for (auto& light : m_lights)
            {
                app->getRenderSystem()->renderSphere(light.obj->transform().position, 0.1f, 25 , light.cmp->color);
            }
        }
#endif

    }
}

