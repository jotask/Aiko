#include "sandbox.h"

#include "components/camera_component.h"
#include "components/compute_shader_component.h"
#include "components/light_component.h"
#include "components/mesh_component.h"
#include "components/model_component.h"
#include "components/particle_emitter_component.h"
#include "components/sprite_component.h"
#include "models/camera.h"
#include "models/game_object.h"
#include "models/mesh_factory.h"
#include "models/texture_factory.h"
#include "systems/system_connector.h"
#include "types/color.h"

#include <application/application.h>
#include <core/random.h>

#include <aiko_includes.h>
#include <chrono>
#include <cmath>

namespace sb
{

    namespace
    {
        constexpr bool s_enableMeshTests = true;
        constexpr bool s_enableComponentTests = true;
        constexpr bool s_enablePrimitivesTests = true;
        constexpr bool s_enableLightTests = true;
        constexpr bool s_enableParticleTests = true;
        constexpr bool s_enableComputeTests = false;
    }

    void Sandbox::init()
    {

        auto* camera = Instantiate("Camera");
        auto* cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Fly);
		camera->transform().position = { 0.0f, 1.0f, 2.5f };
		cam->getCamera().position = camera->transform().position;

        if constexpr (s_enableMeshTests == true)
        {
            initMeshes();
        }

        if constexpr (s_enableComponentTests == true)
        {
            initComponents();
        }

        if constexpr (s_enableParticleTests == true)
        {
            initParticles();
        }

        if constexpr (s_enableComputeTests == true)
        {
            initCompute();
        }

    }

    void Sandbox::update()
    {
        if constexpr (s_enableComponentTests == true)
        {
            updateComponents();
        }
        if constexpr (s_enableLightTests == true)
        {
            updateLights();
        }
        if constexpr (s_enableComputeTests == true)
        {
            updateCompute();
        }
    }

    void Sandbox::render()
    {

        if constexpr (s_enablePrimitivesTests == true)
        {
            renderPrimitives();
        }

        if constexpr (s_enableLightTests == true)
        {
            renderLights();
        }

    }
    void Sandbox::connect(aiko::SystemConnector* systemConnector)
    {
        BIND_SYSTEM_REQUIRED(aiko::RenderSystem, systemConnector, m_renderSystem);
    }

    void Sandbox::initCompute()
    {

        auto root = Instantiate("ComputeTests");

        auto textureCompute = Instantiate(root, "ComputeTexture");

        textureCompute->transform().position = { 0.0f, 0.0f, 3.0f };
        textureCompute->transform().rotation = { 0.0f, 0.0f, 0.0f };
        textureCompute->transform().scale = { 1.0f, 1.0f, 1.0f };

        auto compute = textureCompute->addComponent<aiko::ComputeShaderComponent>();
        compute->load("gradient");
        compute->setUseOutputTexture(true);
        compute->setOutputSize(512, 512);
        compute->setExecutionMode(aiko::ComputeShaderComponent::ComputeExecutionMode::Continuous);
        compute->setUpdateInterval(0.0f);

        auto sprite = textureCompute->addComponent<aiko::SpriteComponent>();
        sprite->load(aiko::texture::factory::generateBlank(512, 512));

        auto& material = sprite->getMaterial();
        material.lit = false;
        material.useVertexColor = false;

        // Compute Read back

        auto readbackCompute = Instantiate(root, "ComputeReadback");
        m_computeReadback = readbackCompute->addComponent<aiko::ComputeShaderComponent>();
        m_computeReadback->load("compute_test");
        m_computeReadback->setElementCount(64);
        m_computeReadback->setExecutionMode(aiko::ComputeShaderComponent::ComputeExecutionMode::Once);

    }

    void Sandbox::initMeshes()
    {

        auto root = Instantiate("MeshesTests");

        auto go1 = Instantiate(root, "Church");
        go1->transform().position = { 0.0f, 0.0f, -15.0f };
        go1->transform().rotation = { 0.0f, 0.0f, 0.0f };
        go1->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto model1 = go1->addComponent<aiko::ModelComponent>();
        model1->load("church.obj");

        auto go2 = Instantiate(root, "Barracks");
        go2->transform().position = { -50.0f, 0.0f, -15.0f };
        go2->transform().rotation = { 0.0f, 0.0f, 0.0f };
        go2->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto model2 = go2->addComponent<aiko::ModelComponent>();
        model2->load("barracks.obj");

        auto go3 = Instantiate(root, "Watermill");
        go3->transform().position = { 50.0f, 0.0f, -15.0f };
        go3->transform().rotation = { 0.0f, 0.0f, 0.0f };
        go3->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto model3 = go3->addComponent<aiko::ModelComponent>();
        model3->load("watermill.obj");

        auto go4 = Instantiate(root, "Robot");
        go4->transform().position = { 0.0f, 0.0f, 5.0f };
        go4->transform().rotation = { 0.0f, 0.0f, 0.0f };
        const float scale = 0.25f;
        go4->transform().scale = { scale, scale, scale };
        auto model4 = go4->addComponent<aiko::ModelComponent>();
        model4->load("robot.glb");
    }

    void Sandbox::initComponents()
    {

        const aiko::MeshAsset defaultCube = aiko::mesh::factory::generateCube();

        constexpr float zAxis = 2.5f;

        auto root = Instantiate("ComponentsTests");

        m_go1 = Instantiate(root, "Cube1");
        m_go1->transform().position = { 1.0f, 0.0f, zAxis };
        m_go1->transform().rotation = { 0.0f, 0.0f, 0.0f };
        m_go1->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto mesh1 = m_go1->addComponent<aiko::MeshComponent>();
        mesh1->load(defaultCube);

        m_go2 = Instantiate(root, "Cube2");
        m_go2->transform().position = { -1.0f, 0.0f, zAxis };
        m_go2->transform().rotation = { 0.0f, 0.0f, 0.0f };
        m_go2->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto mesh2 = m_go2->addComponent<aiko::MeshComponent>();
        mesh2->load(defaultCube);

        m_texture = Instantiate(root, "Texture");
        m_texture->transform().position = { 0.0f, -0.55f, zAxis };
        m_texture->transform().rotation = { 0.0f, 0.0f, 0.0f };
        m_texture->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto mesh3 = m_texture->addComponent<aiko::SpriteComponent>();
        mesh3->load("texel_checker.png");

        m_texturePbo = Instantiate(root, "PboTexture");
        m_texturePbo->transform().position = { 0.0f, 0.55f, zAxis };
        m_texturePbo->transform().rotation = { 0.0f, 0.0f, 0.0f };
        m_texturePbo->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto mesh4 = m_texturePbo->addComponent<aiko::SpriteComponent>();
        mesh4->load(aiko::texture::factory::generateBlank(128, 128));

        aiko::MaterialAsset& material = mesh4->getMaterial();
        material.lit = false;
        // material.setTextureFilter(aiko::TextureFilter::Nearest, aiko::TextureFilter::Nearest);
        // material.setTextureMipFilter(aiko::TextureMipFilter::None);
        // material.setTextureWrapMode(aiko::TextureWrapMode::Clamp, aiko::TextureWrapMode::Clamp);
    }

    void Sandbox::initLights()
    {

        auto root = Instantiate("LightTests");

        constexpr float radiusSpawn = 10.0f;
        for (size_t i = 0 ; i < 10; ++i )
        {
            auto* obj = Instantiate(root, "Light");
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
    }

    void Sandbox::initParticles()
    {

        auto root = Instantiate("ParticlesTests");

        auto ps = Instantiate(root, "Particle Emitter");
        ps->transform().position = { 0.0f, 0.0f, -3.5f };

        auto emitter = ps->addComponent<aiko::ParticleEmitterComponent>();
        emitter->setMaxParticles(1024 * 1024);
        emitter->setLifetime(1.0f);
        emitter->setDirection({0.0f, 1.0f, 0.0f});
        emitter->setDirectionRandomness(0.85f);
        emitter->setGravity({0.0f, -1.5f, 0.0f});
        emitter->setStartSpeed(2.0f);
        emitter->setSpawnRate(1000.0f * 100.0f);
        emitter->setPlaying(true);
        emitter->setSpawnShape(aiko::ParticleEmitterComponent::ParticleSpawnShape::Point);
        constexpr float spawnSize = 2.5f;
        emitter->setSpawnRadius(spawnSize);
        emitter->setSpawnBoxExtents({spawnSize, spawnSize, spawnSize});
        emitter->requestReset();
    }

    void Sandbox::updateComponents()
    {
        static float angle = 0.0f;
        angle += 25.0f * getDeltaTime();
        angle = fmod(angle, 360.0f);
        m_go1->transform().rotation = {  angle, 0.0f, 0.0f };
        m_go2->transform().rotation = { -angle, 0.0f, 0.0f };

        aiko::SpriteComponent* cmp = m_texturePbo->getComponent<aiko::SpriteComponent>();

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

    void Sandbox::updateLights()
    {
        for (auto& light : m_lights)
        {
            light.angle += 1.0f * getDeltaTime();
            aiko::vec3 pos = {std::sin(light.angle), 0.0f, std::cos(light.angle)};
            light.obj->transform().position = pos;
        }
    }

    void Sandbox::renderPrimitives()
    {
        constexpr const float SIZE = 1.0f;

        m_renderSystem->renderPoint({ 1.0f, 1.0f, 0.0f });
        m_renderSystem->renderLine({ -2.0f, -1.0f, 0.0f }, { 2.0f, -1.0f, 0.0f });
        m_renderSystem->renderRectangle({ 0.0f, 1.0f, 0.0f }, SIZE);
        m_renderSystem->renderCircle({ 2.0f, 1.0f, 0.0f }, SIZE, 32);
        m_renderSystem->renderTriangle({ -1.0f, 1.0f, 0.0f }, SIZE);
        m_renderSystem->renderNgon({ -2.0f, 1.0f, 0.0f }, SIZE, 6);
        m_renderSystem->renderCube({ 1.0f, 0.0f, 0.0f }, SIZE);
        m_renderSystem->renderPyramid({ 0.0f, 0.0f, 0.0f }, SIZE);
        m_renderSystem->renderSphere({ -1.0f, 0.0f, 0.0f }, SIZE);
        m_renderSystem->renderCylinder({ -2.0f, 0.0f, 0.0f }, SIZE, 6);
        m_renderSystem->renderPolygon({ -3.0f, 0.0f, 0.0f }, SIZE, 6, 6);
        m_renderSystem->renderTorus({ 2.0f, 0.0f, 0.0f }, SIZE);
        m_renderSystem->renderKnot({ 3.0f, 0.0f, 0.0f }, SIZE);
        m_renderSystem->renderGrid({ 0.0f, -2.0f, 0.0f }, SIZE, {10, 10});

    }

    void Sandbox::renderLights()
    {
        for (auto& light : m_lights)
        {
            m_renderSystem->renderSphere(light.obj->transform().position, 0.1f, 25 /*, light.cmp->color*/ );
        }
    }

    void Sandbox::updateCompute()
    {
        if (m_computeReadback == nullptr || m_computeReadbackPrinted == true || m_computeReadback->hasReadback() == false)
        {
            return;
        }

        const auto& result = m_computeReadback->getLastReadback();

        constexpr uint32_t ElementCount = 64;
        const size_t expectedSize = ElementCount * sizeof(aiko::vec4);

        AIKO_ASSERT(result.data.size() == expectedSize, "Invalid compute readback size");

        const auto* values = reinterpret_cast<const aiko::vec4*>(result.data.data());

        for (uint32_t i = 0; i < ElementCount; ++i)
        {
            aiko::logger::Log::info("Compute[{%zu}] = ({%.2f}, {%.2f}, {%.2f}, {%.2f})", i, values[i].x, values[i].y, values[i].z, values[i].w);
        }
        m_computeReadbackPrinted = true;
    }

}

