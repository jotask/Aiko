#pragma once

#include "aiko_types.h"
#include "fluid_hash_grid.h"
#include "particle_emitter.h"
#include "shape.h"
#include "sph_types.h"
#include "spring.h"

#include <unordered_map>

namespace sph
{

    class Simulation
    {

    public:

        void init();
        void update();

        void updateEmitters(float dt, aiko::vector<SPHParticle>& spawnedParticles);

        aiko::vector<SPHParticle>& particles() { return m_particles; }
        const aiko::vector<SPHParticle>& particles() const { return m_particles; }
        const aiko::vector<ParticleEmitter>& emitters() const { return m_emitters; }

        const SPHParameters& parameters() const { return m_parameters; }
        SPHParameters& parameters() { return m_parameters; }

        const WorldBounds& bounds() const { return m_bounds; }

        const aiko::vector<Shape>& shapes() const { return m_shapes; }
        aiko::vector<Shape>& shapes() { return m_shapes; }

        void neighboursSearch(const aiko::vec3& mousePosition);

        ParticleEmitter* createParticleEmitter(const EmitterSettings);

    private:

        static constexpr float VelocityDamping = 1.0f;

        aiko::vector<SPHParticle> m_particles;

        const WorldBounds m_bounds
        {
            .position = {0.0f, 0.0f, 0.0f},
            .size = {32.0f, 32.0f, 0.0f}
        };

        SPHParameters m_parameters;
        FluidHashGrid m_hashGrid;

        std::unordered_map<aiko::u64, Spring> m_springs;

        aiko::vector<ParticleEmitter> m_emitters;
        aiko::vector<Shape> m_shapes;

        void predictPositions(float dt);
        void computeNextVelocity(float dt);
        void viscosity(float dt);
        void adjustSpring(float dt);
        void springDisplacement(float dt);
        void doubleDensityRelaxation(float dt);
        void applyGravity(float dt);

        void handleOneWayCoupling();
        void handleStickiness(float dt);

        void worldBoundary();

    };

}
