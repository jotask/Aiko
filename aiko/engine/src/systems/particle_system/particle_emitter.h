#pragma once

#include <memory>
#include <vector>

#include "aiko_types.h"
#include "systems/particle_system/particle.h"

namespace aiko
{

    class ParticleEmitter
    {
        friend class ParticleSystem;
    public:
        ParticleEmitter();
        ParticleEmitter(size_t numerOfParticles);
    protected:
        void reset();
        void update();
        void render();
    private:

        static constexpr std::size_t MAX_PARTICLES = 100;

        std::vector<Particle> m_particles;

    };

}
