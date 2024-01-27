#include "entity_component_system.h"

#include <stdexcept>
#include <memory>
#include <cassert>

// TODO Please delete, temporally as initial code
#include <raylib.h>

#include "modules/module_connector.h"
#include "modules/scene_module.h"
#include "components/transform_component.h"
#include "components/component_renderer.h"
#include "render_system.h"
#include "models/light.h"
#include "models/vector.h"

//  TODO TMP
#include "modules/renderer/render_component_texture.h"

namespace aiko
{
    
    void RenderSystem::Particle::checkBounds(vec2 bounds)
    {
        if (position.x < 0)
        {
            velocity.x *= -1.0f;
            position.x = 0;
        }
        if (position.x > bounds.x)
        {
            velocity.x *= -1.0f;
            position.x = bounds.x;
        }

        if (position.y < 0)
        {
            velocity.y *= -1.0f;
            position.y = 0;
        }
        if (position.y > bounds.y)
        {
            velocity.y *= -1.0f;
            position.y = bounds.y;
        }

    }

    RenderSystem::RenderSystem()
        : m_seeds()
        , m_rendererComponentTexture(nullptr)
    {
    }
    
    void RenderSystem::init()
    {
        m_rendererComponentTexture = (RenderComponentTexture*)m_renderModule->GetRenderComponent();
        // TODO Again, please delete me
        RegenerateSeeds();

        auto size = m_renderModule->getDisplaySize();
        m_target = LoadRenderTexture(size.x, size.y);

        m_shader = LoadShader(0, "C:/Users/j.iznardo/Documents/Aiko/assets/shaders/aiko_distance.fs");

        positionLoc = GetShaderLocation(m_shader, "particles");

        assert(positionLoc != -1);

    }
    
    void RenderSystem::update()
    {

        if ( IsWindowResized() == true && IsWindowFullscreen() == false )
        {
            auto screenWidth = GetScreenWidth();
            auto screenHeight = GetScreenHeight();
            UnloadRenderTexture(m_target);
            m_target = LoadRenderTexture(screenWidth, screenHeight);
            RegenerateSeeds();
        }

        if (IsKeyPressed(KEY_F2))
        {
            RegenerateSeeds();
        }
        updatSeeds();

        glm::vec2 position;
        std::vector<glm::vec2> positions;
        for(auto tmp : m_seeds)
        {
            positions.push_back(tmp.position);
        }
        SetShaderValueV(m_shader, positionLoc, positions.data(), SHADER_UNIFORM_VEC2, positions.size());

        // We are done, render to the texture
        BeginTextureMode(m_target);
        ClearBackground(BLACK);
        DrawRectangle(0, 0, m_target.texture.width, m_target.texture.height, BLACK);
        EndTextureMode();

    }

    void RenderSystem::updatSeeds()
    {
        const auto frameTime = GetFrameTime();
        for(auto& p : m_seeds)
        {
            p.position += p.velocity * frameTime;
            p.checkBounds(m_renderModule->getDisplaySize());
        }
    }
    
    aiko::AikoPtr<Mesh> RenderSystem::createMesh()
    {
        auto mesh = std::make_unique<Mesh>();
        return mesh;
    }
    
    aiko::AikoPtr<Light> RenderSystem::createLight()
    {
        auto light = std::make_unique<Light>();
        return light;
    }
    
    void RenderSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        m_renderModule = moduleConnector->find<RenderModule>();
    }
    
    void RenderSystem::RegenerateSeeds()
    {

        auto screen = m_renderModule->getDisplaySize();
    
        float deltaTime = GetFrameTime();
    
        m_seeds = std::vector<Particle>(nParticles);
    
        constexpr int VELOCITY = 10;

        for (int i = 0; i < nParticles; i++)
        {
            Particle particle;
            particle.position = { GetRandomValue(0, screen.x), GetRandomValue(0, screen.y) };
            particle.velocity = { GetRandomValue(-VELOCITY, VELOCITY), GetRandomValue(-VELOCITY, VELOCITY)};
            m_seeds[i] = particle;
        }
    }
    
    void RenderSystem::add(Light* light)
    {
        switch (light->m_type)
        {
        case Light::Type::Ambient:
            {
    
            }
            break;
        case Light::Type::Directional:
            {
    
            }
            break;
        case Light::Type::Point:
            {
    
            }
            break;
        default:
            throw new std::exception();
            break;
        }
    }
    
    void RenderSystem::render(MeshComponent* mesh)
    {
        BeginShaderMode(m_shader);
        DrawTextureEx(m_target.texture, { 0.0f, 0.0f }, 0.0f, 1.0f, WHITE);
        EndShaderMode();
    }

}
