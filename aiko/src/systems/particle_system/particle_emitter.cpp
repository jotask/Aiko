#include "particle_emitter.h"

#include <algorithm>

namespace aiko
{

    ParticleEmitter::ParticleEmitter()
        : ParticleEmitter(MAX_PARTICLES)
    {

    }

    ParticleEmitter::ParticleEmitter(size_t numerOfParticles)
        : m_particles(numerOfParticles)
    {
    }

    void ParticleEmitter::reset()
    {
        m_particles.clear();
    }

    void ParticleEmitter::update()
    {
        std::for_each(m_particles.begin(), m_particles.end(), [](const Particle& p) { });
    }

    void ParticleEmitter::render()
    {
        std::for_each(m_particles.begin(), m_particles.end(), [](const Particle& p) {});
    }

}
