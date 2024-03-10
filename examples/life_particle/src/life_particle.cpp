#include "life_particle.h"

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "systems/render_system.h"
#include "core/utils.h"

namespace life
{
    void LifeParticle::init()
    {
        Application::init();
        for (size_t i = 0 ; i < MAX_NUM_TOTAL_PARTICLES; ++i)
        {
            m_particles.emplace_back(Particle());
            auto size = getViewportSize();
            float x = aiko::utils::getRandomValue(0.0f, size.x);
            float y = aiko::utils::getRandomValue(0.0f, size.y);
            m_particles.back().position = { x, y };
        }

        // Shader
        {
            if (m_shader != nullptr)
            {
                unloadShader(m_shader);
                m_shader = nullptr;
            }

            const std::string GLOBAL_PATH = "C:/Users/j.iznardo/Documents/Aiko/assets/shaders/";
            const std::string path = GLOBAL_PATH + "aiko_particle.fs";
            m_shader = loadShader(nullptr, path.c_str());

            iParticlesLoc = m_shader->getShaderLocation("particles");
            iRadiusLoc = m_shader->getShaderLocation("radius");
            iResolutionLoc = m_shader->getShaderLocation("iResolution");

        }
    }

    void LifeParticle::update()
    {
        Application::update();
        for (auto& p : m_particles)
        {
            p.update();
        }

        // update shader

        std::vector<aiko::vec2> pos;
        for (auto& p : m_particles)
        {
            pos.push_back(p.position);
        }

        auto size = getViewportSize();
        m_shader->setShaderValue(iResolutionLoc, size);
        // m_shader->setShaderValue(iRadiusLoc, particleRadius);
        // m_shader->setShaderValue(iParticlesLoc, pos );
    }

    void LifeParticle::render()
    {
        Application::render();
        static aiko::RenderSystem* rs = m_aiko->getSystem<aiko::RenderSystem>();
        for (auto& p : m_particles)
        {

        }
        auto* texture = rs->getTargetTexture();
        auto* renderer = getRenderSystem();
        rs->render(*texture, m_shader);
    }

}

