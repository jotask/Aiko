#include "entity_component_system.h"

#include <stdexcept>
#include <memory>
#include <cassert>
#include <iostream>
#include <format>
#include <chrono>
#include <chrono>
#include <iostream>

// TODO Please delete, temporally as initial code
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>

#include "aiko_types.h"
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
        : m_particles()
        , m_rendererComponentTexture(nullptr)
    {
    }
    
    void RenderSystem::init()
    {
        m_rendererComponentTexture = (RenderComponentTexture*)m_renderModule->GetRenderComponent();
        // TODO Again, please delete me
        RegenerateSeeds();
            
        iTime = 0.0f;

        currentShader = 0; // GetRandomValue(0, s_shaders.size());
        RefreshShader(true);

    }

    std::vector<std::string> RenderSystem::s_shaders =
    {
        "aiko_shadertoy_coastal_fractal",
        "aiko_shadertoy_cyber_fuji",
        "aiko_shadertoy_fractal_pyramid",
        "aiko_shadertoy_phanton_start",
        "aiko_shadertoy_sun",
        "aiko_shadertoy_sun_mandala",
        "aiko_shadertoy_fractal_lines",
        "aiko_shadertoy_mandala_art_of_code",
        "aiko_shadertoy_hexaflower",
        "aiko_shadertoy_rosaic",
        "aiko_shadertoy_test_fractal",
        "aiko_shadertoy_nunu_mandala",
        "aiko_shadertoy_fractal_mandala_twtich",
        "aiko_shadertoy_fractal_circles",
        "aiko_shadertoy_mandala_start_pattern",
        "aiko_shadertoy_weird_twitch",
        "aiko_shadertoy_mandala",
        "aiko_shadertoy_a_nice_mandala",
        "aiko_shadertoy_mandala_flowers",
        "aiko_shadertoy_happy_seigaiha_mandala",
        "aiko_shadertoy",
        "aiko_shadertoy_fractal",
        "aiko_shadertoy_fractal_infinite_1",
        "aiko_shadertoy_fractal_infinite_2",
        "aiko_shadertoy_fractal_infinite_spheres",
        "aiko_shadertoy_happy",
    };

    void RenderSystem::NextShader()
    {
        currentShader++;
        currentShader %= s_shaders.size();
        RefreshShader();
    }

    void RenderSystem::RandomShader()
    {
        currentShader = GetRandomValue(0, s_shaders.size());
        RefreshShader();
    }

    void RenderSystem::PrevShader()
    {
        currentShader--;
        currentShader %= s_shaders.size();
        RefreshShader();
    }

    void RenderSystem::RefreshShader(bool first)
    {
        if (first == false)
        {
            UnloadShader(m_shader);
        }
       
        const std::string path = GLOBAL_PATH + s_shaders[currentShader].c_str() + EXTENSION;
        m_shader = LoadShader(nullptr, path.c_str());

        iResolutionLoc = GetShaderLocation(m_shader, "iResolution");
        iTimeLoc = GetShaderLocation(m_shader, "iTime");
        iTimeDeltaLoc = GetShaderLocation(m_shader, "iTimeDelta");
        iFrameRateLoc = GetShaderLocation(m_shader, "iFrameRate");
        iFrameLoc = GetShaderLocation(m_shader, "iFrame");
        iChannelTimeLoc = GetShaderLocation(m_shader, "iChannelTime");
        iChannelResolutionLoc = GetShaderLocation(m_shader, "iChannelResolution");
        iMouseLoc = GetShaderLocation(m_shader, "iMouse");
        iDateLoc = GetShaderLocation(m_shader, "iDate");

        // assert(positionLoc != -1 && nParticlesLoc != -1 && resolutionLoc != -1 && radiusLoc != -1 );
        // assert(iResolutionLoc != -1 && iTimeLoc != -1 && iTimeDeltaLoc != -1 && iFrameRateLoc != -1 && iFrameLoc != -1 && iChannelTimeLoc != -1 && iChannelResolutionLoc != -1 && iMouseLoc != -1 && iDateLoc != -1);

    }

    void RenderSystem::update()
    {

        // check for alt + enter
        if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
        {
            ToggleFullscreen();
        }

        if ( IsWindowResized() == true && IsWindowFullscreen() == false )
        {
            RegenerateSeeds();
        }

        if (IsKeyPressed(KEY_F2))
        {
            RegenerateSeeds();
        }

        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_RIGHT))
        {
            NextShader();
        }
        else if (IsKeyPressed(KEY_LEFT))
        {
            PrevShader();
        }
        else if (IsKeyPressed(KEY_R))
        {
            RandomShader();
        }

        updatSeeds();

        glm::vec2 position;
        std::vector<glm::vec2> positions;
        for(auto tmp : m_particles)
        {
            positions.push_back(tmp.position);
        }

        // Please fix me
        {

            iTime += GetFrameTime();

            iTimeDelta = GetFrameTime();
            iFrameRate = GetFrameTime();

            iFrame++;                

            // float     iChannelTime[4];       
            // vec3      iChannelResolution[4]; 

            auto mouse = GetMousePosition();
            iMouse = vec4(mouse.x, mouse.y, IsMouseButtonPressed(MOUSE_BUTTON_RIGHT), IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) );

            typedef std::chrono::system_clock Clock;
            auto now = Clock::now();
            std::time_t now_c = Clock::to_time_t(now);
            struct tm* parts = std::localtime(&now_c);
            iDate = vec4( 1900 + parts->tm_year , 1 + parts->tm_mon , parts->tm_mday, time(NULL) );
        }

        SetShaderValueV(m_shader, positionLoc, positions.data(), SHADER_UNIFORM_VEC2, positions.size());
        SetShaderValue(m_shader, nParticlesLoc, &nParticles, SHADER_UNIFORM_INT);
        SetShaderValue(m_shader, resolutionLoc, &m_renderModule->getDisplaySize(), SHADER_UNIFORM_VEC2);
        SetShaderValue(m_shader, radiusLoc, &radius, SHADER_UNIFORM_FLOAT);

        SetShaderValue(m_shader, iResolutionLoc, &m_renderModule->getDisplaySize(), SHADER_UNIFORM_VEC3);   // viewport resolution (in pixels)
        SetShaderValue(m_shader, iTimeLoc, &iTime, SHADER_UNIFORM_FLOAT);                                   // shader playback time (in seconds)
        SetShaderValue(m_shader, iTimeDeltaLoc, &iTimeDelta, SHADER_UNIFORM_FLOAT);                         // render time (in seconds)
        SetShaderValue(m_shader, iFrameRateLoc, &iFrameRate, SHADER_UNIFORM_FLOAT);                         // shader frame rate
        SetShaderValue(m_shader, iFrameLoc, &iFrame, SHADER_UNIFORM_INT);                                   // shader playback frame
        SetShaderValue(m_shader, iChannelTimeLoc, &iChannelTime, SHADER_UNIFORM_FLOAT);                     // channel playback time (in seconds)
        SetShaderValue(m_shader, iChannelResolutionLoc, &iChannelResolution, SHADER_UNIFORM_VEC3);          // channel resolution (in pixels)
        SetShaderValue(m_shader, iMouseLoc, &iMouse, SHADER_UNIFORM_VEC4);                                  // mouse pixel coords. xy: current (if MLB down), zw: click
        SetShaderValue(m_shader, iDateLoc, &iDate, SHADER_UNIFORM_VEC4);                                    // (year, month, day, time in seconds)

    }

    void RenderSystem::render()
    {

        BeginShaderMode(m_shader);
        DrawTextureEx(m_rendererComponentTexture->GetRendererTexture().texture, { 0.0f, 0.0f }, 0.0f, 1.0f, WHITE);
        EndShaderMode();

        /*
        if (::ImGui::Begin("RenderSystem"))
        {
            if (::ImGui::DragFloat("Radius", &radius, 0.5f, 0.0f, 100.0f, "%.3f p"))
            {

            }
            if (::ImGui::DragFloat("Velocity", &all_velocity, 0.5f, 0.0f, 1000.0f, "%.3f p/s"))
            {
                for (auto& p : m_particles)
                {
                    p.speed = all_velocity;
                }
            }
            if (::ImGui::DragInt("Particles", &nParticles, 1, 0, nMaxParticles))
            {
                RegenerateSeeds();
            }
            if (::ImGui::Button("Randomize Velocity"))
            {
                for (auto& p : m_particles)
                {
                    p.velocity = { GetRandomValue(-Particle::VELOCITY, Particle::VELOCITY), GetRandomValue(-Particle::VELOCITY, Particle::VELOCITY) };
                }
            }
            ::ImGui::End();
        }
        */
    }

    void RenderSystem::updatSeeds()
    {
        const auto frameTime = GetFrameTime();
        for(auto& p : m_particles)
        {
            p.position += (p.velocity * p.speed) * frameTime;
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

    
        constexpr int VELOCITY = 1;

        vec2 screen = m_renderModule->getDisplaySize();

        m_particles.clear();
        m_particles.reserve(nParticles);

        for (int i = 0; i < nParticles; i++)
        {
            Particle particle;
            particle.speed = all_velocity;
            particle.position = { GetRandomValue(0, screen.x), GetRandomValue(0, screen.y) };
            particle.velocity = { GetRandomValue(-Particle::VELOCITY, Particle::VELOCITY), GetRandomValue(-Particle::VELOCITY, Particle::VELOCITY)};
            m_particles.push_back(particle);
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

    }

}
