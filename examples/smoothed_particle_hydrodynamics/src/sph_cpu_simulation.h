#pragma once

#include "fluid_hash_grid.h"
#include "shape.h"
#include "sph_types.h"
#include "spring.h"

#include <unordered_map>

namespace sph
{

    class SPHCpuSimulation
    {
    public:

        void init(const aiko::vector<SPHParticle>& particles, float smoothingRadius);

        void update(const SPHParameters& parameters, const WorldBounds& bounds, const aiko::vector<Shape>& shapes);

        void spawnParticles(const aiko::vector<SPHParticle>& particles);

        void neighboursSearch(const aiko::vec3& mousePosition, float smoothingRadius);

        const aiko::vector<SPHParticle>& particles() const { return m_particles; }

        uint32_t particleCount() const { return static_cast<uint32_t>(m_particles.size()); }

    private:

        void predictPositions(float dt);

        void computeNextVelocity(const SPHParameters& parameters, float dt);

        void viscosity(const SPHParameters& parameters, float dt);

        void adjustSpring(const SPHParameters& parameters, float dt);

        void springDisplacement(const SPHParameters& parameters, float dt);

        void doubleDensityRelaxation(const SPHParameters& parameters, float dt);

        void applyGravity(const SPHParameters& parameters, float dt);

        void handleOneWayCoupling(const SPHParameters& parameters, const aiko::vector<Shape>& shapes);

        void handleStickiness(const SPHParameters& parameters, const aiko::vector<Shape>& shapes, float dt);

        void worldBoundary(const SPHParameters& parameters, const WorldBounds& bounds);

        void invalidateSpringsForParticle(size_t particleIndex);

        aiko::vector<SPHParticle> m_particles;

        FluidHashGrid m_hashGrid;

        std::unordered_map<aiko::u64, Spring> m_springs;

        size_t m_nextParticleSlot = 0;
    };

}
