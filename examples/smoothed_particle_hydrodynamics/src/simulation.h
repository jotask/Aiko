#pragma once

#include "aiko_types.h"
#include "fluid_hash_grid.h"
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

        const SPHParameters& parameters() const { return m_parameters; }
        const WorldBounds& bounds() const { return m_bounds; }

        void neighboursSearch(const aiko::vec3& mousePosition);

    private:

        static constexpr float VelocityDamping = 1.0f;
        static constexpr float BoundaryDamping = 0.8f;

        std::array<SPHParticle, N_PARTICLES> m_particles;

        const WorldBounds m_bounds
        {
            .position = {0.0f, 0.0f, 0.0f},
            .size = {5.0f, 5.0f, 0.0f}
        };

        SPHParameters m_parameters;
        FluidHashGrid m_hashGrid;

        void predictPositions(float dt);
        void computeNextVelocity(float dt);
        void applyGravity(float dt);

        void worldBoundary();

    };

}
