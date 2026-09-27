#pragma once

#include "aiko_types.h"
#include "sph_types.h"

#include <array>

namespace sph
{

    class Simulation
    {

    public:
        static constexpr aiko::u64 N_PARTICLES = 1024;

        void init();
        void update();

        const std::array<SPHParticle, N_PARTICLES>& particles() const
        {
            return m_particles;
        }

    private:


        static constexpr float VelocityDamping = 1.0f;

        std::array<SPHParticle, N_PARTICLES> m_particles;

        void predictPositions(float dt);
        void computeNextVelocity(float dt);

    };

}
