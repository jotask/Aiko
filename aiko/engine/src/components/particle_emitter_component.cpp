#include "particle_emitter_component.h"

namespace aiko
{
    
    ParticleEmitterComponent::ParticleEmitterComponent()
        : Component("ParticleEmitterComponent")
    {
    
    }

    bool ParticleEmitterComponent::consumeResetRequest()
    {
        const bool value = m_resetRequested;
        m_resetRequested = false;
        return value;
    }

}
