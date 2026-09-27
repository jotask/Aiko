#pragma once

#include "aiko_types.h"
#include "fluid_hash_grid.h"
#include "particle_emitter.h"
#include "sph_types.h"

namespace sph
{

    class Simulation
    {

    public:
        static constexpr aiko::u64 N_PARTICLES = 1024;

        void init();
        void update();

        aiko::vector<SPHParticle>& particles() { return m_particles; }
        const aiko::vector<SPHParticle>& particles() const { return m_particles; }
        const aiko::vector<ParticleEmitter>& emitters() const { return m_emitters; }

        const SPHParameters& parameters() const { return m_parameters; }
        const WorldBounds& bounds() const { return m_bounds; }

        void neighboursSearch(const aiko::vec3& mousePosition);

        ParticleEmitter* createParticleEmitter(const EmitterSettings);

    private:

        static constexpr float VelocityDamping = 1.0f;

        aiko::vector<SPHParticle> m_particles;

        const WorldBounds m_bounds
        {
            .position = {0.0f, 0.0f, 0.0f},
            .size = {8.0f, 6.0f, 0.0f}
        };

        SPHParameters m_parameters;
        FluidHashGrid m_hashGrid;

        aiko::vector<ParticleEmitter> m_emitters;

        void predictPositions(float dt);
        void computeNextVelocity(float dt);
        void viscosity(float dt);
        void doubleDensityRelaxation(float dt);
        void applyGravity(float dt);

        void worldBoundary();

    };

}
