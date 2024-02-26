#include "entity_component_system.h"

#include <stdexcept>
#include <memory>
#include <cassert>
#include <iostream>
#include <format>
#include <iostream>

// TODO Please delete, temporally as initial code
#include "core/libs.h"

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

    RenderSystem::RenderSystem()
        : m_rendererComponentTexture(nullptr)
    {
    }
    
    void RenderSystem::init()
    {
        m_rendererComponentTexture = (RenderComponentTexture*)m_renderModule->GetRenderComponent();
    }

    void RenderSystem::update()
    {
        // check for alt + enter
        if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
        {
            ToggleFullscreen();
        }
    }

    void RenderSystem::render()
    {

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

    RenderTexture2D* RenderSystem::getTargetTexture() const
    {
        return m_renderModule->getRenderTexture();
    }

}
