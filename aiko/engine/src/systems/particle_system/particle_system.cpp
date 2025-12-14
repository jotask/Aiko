#include "particle_system.h"

#include "modules/module_connector.h"
#include "modules/render_module.h"

namespace aiko
{

    void ParticleSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule)
    }
    
    void ParticleSystem::init()
    {

    }
    
    void ParticleSystem::update()
    {

    }

    void ParticleSystem::render()
    {

    }

    ParticleEmitter* ParticleSystem::createEmitter(size_t nParticle)
    {
        m_emitters.emplace_back(ParticleEmitter());
        ParticleEmitter* particleEmitter = &m_emitters.back();
        return particleEmitter;
    }

}
