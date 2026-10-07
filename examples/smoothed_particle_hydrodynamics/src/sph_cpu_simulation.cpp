#include "sph_cpu_simulation.h"

#include "intrumentor/profiler.h"
#include "math/math.h"

#include <algorithm>
#include <limits>

namespace sph
{

    void SPHCpuSimulation::init(const aiko::vector<SPHParticle>& particles, float smoothingRadius)
    {
        AIKO_ASSERT(particles.size() <= MaxCpuParticles, "CPU SPH initial particle count exceeds shared particle capacity");

        AIKO_ASSERT(smoothingRadius > 0.0f, "CPU SPH smoothing radius must be positive");

        m_particles = particles;

        m_springs.clear();

        if (m_particles.size() < MaxCpuParticles)
        {
            m_nextParticleSlot = m_particles.size();
        }
        else
        {
            m_nextParticleSlot = 0;
        }

        m_hashGrid.init(&m_particles, smoothingRadius);
    }

    void SPHCpuSimulation::update(const SPHParameters& parameters, const WorldBounds& bounds, const aiko::vector<Shape>& shapes)
    {
        AIKO_FUNCTION_PROFILE

        AIKO_PLOT("SPH CPU Particles", static_cast<double>( m_particles.size()));

        AIKO_PLOT("SPH CPU Springs", static_cast<double>( m_springs.size()));

        const float dt = parameters.solverDeltaTime;

        if (dt <= 0.0f)
        {
            return;
        }

        applyGravity(parameters, dt);

        m_hashGrid.clearGrid();
        m_hashGrid.mapParticlesToCell();

        viscosity(parameters, dt);
        predictPositions(dt);
        adjustSpring(parameters, dt);
        springDisplacement(parameters, dt);
        doubleDensityRelaxation(parameters, dt);
        handleStickiness(parameters, shapes, dt);
        handleOneWayCoupling(parameters, shapes);
        worldBoundary(parameters, bounds);
        computeNextVelocity(parameters, dt);
    }

    void SPHCpuSimulation::spawnParticles(const aiko::vector<SPHParticle>& particles)
    {
        for (const SPHParticle& particle : particles)
        {
            if (m_particles.size() < MaxCpuParticles)
            {
                m_particles.emplace_back(particle);

                m_nextParticleSlot = m_particles.size() % MaxCpuParticles;

                continue;
            }

            AIKO_ASSERT(m_nextParticleSlot < m_particles.size(), "CPU SPH recycle slot out of range");

            invalidateSpringsForParticle(m_nextParticleSlot);

            m_particles[m_nextParticleSlot] = particle;

            m_nextParticleSlot = ( m_nextParticleSlot + 1 ) % MaxCpuParticles;
        }
    }

    void SPHCpuSimulation::invalidateSpringsForParticle(size_t particleIndex)
    {
        for (auto it = m_springs.begin(); it != m_springs.end();)
        {
            const Spring& spring = it->second;

            if (spring.particleA == particleIndex || spring.particleB == particleIndex)
            {
                it = m_springs.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void SPHCpuSimulation::predictPositions(float dt)
    {
        AIKO_FUNCTION_PROFILE

        for (SPHParticle& particle : m_particles)
        {
            particle.prevPosition = particle.position;
            particle.position += particle.velocity * dt;
        }
    }

    void SPHCpuSimulation::computeNextVelocity(const SPHParameters& parameters, float dt)
    {
        AIKO_FUNCTION_PROFILE

        const float maxVelocity = ( parameters.smoothingRadius * parameters.maxStepDisplacementRatio ) / dt;

        const float maxVelocitySquared = maxVelocity * maxVelocity;

        for (SPHParticle& particle : m_particles)
        {
            const aiko::vec3 direction = particle.position - particle.prevPosition;

            particle.velocity = direction * (1.0f / dt) * parameters.velocityDamping;

            const float velocitySquared = aiko::math::lengthSquared(particle.velocity);

            if (velocitySquared <= maxVelocitySquared)
            {
                continue;
            }

            const float velocityLength = aiko::math::length(particle.velocity);

            if (velocityLength > 0.0f)
            {
                particle.velocity *= maxVelocity / velocityLength;
            }
        }
    }

    void SPHCpuSimulation::viscosity(const SPHParameters& parameters, float dt)
    {
        AIKO_FUNCTION_PROFILE

        for (size_t i = 0; i < m_particles.size(); ++i)
        {
            aiko::vector<size_t> neighbours;

            {
                AIKO_ZONE_NAMED("CPU Viscosity Get Neighbours")
                neighbours = m_hashGrid.getNeighbourOfParticlesIdx(i);
            }

            SPHParticle& particleA = m_particles[i];

            {
                AIKO_ZONE_NAMED("CPU Viscosity Solve")

                for (size_t neighbourIndex : neighbours)
                {
                    if (i == neighbourIndex)
                    {
                        continue;
                    }

                    SPHParticle& particleB = m_particles[neighbourIndex];

                    const aiko::vec3 directionNeighbour = particleB.position - particleA.position;

                    const float distance = aiko::math::length(directionNeighbour);

                    const float q = distance / parameters.smoothingRadius;

                    if (q >= 1.0f || distance <= 1e-6f)
                    {
                        continue;
                    }

                    const aiko::vec3 normalizedDirection = directionNeighbour / distance;

                    const float u = aiko::math::dot( particleA.velocity - particleB.velocity, normalizedDirection);

                    if (u <= 0.0f)
                    {
                        continue;
                    }

                    const float term = dt * (1.0f - q) * ( parameters.sigma * u + parameters.beta * u * u );

                    const aiko::vec3 impulse = term * normalizedDirection;

                    particleA.velocity -= impulse * 0.5f;

                    particleB.velocity += impulse * 0.5f;
                }
            }
        }
    }

    void SPHCpuSimulation::adjustSpring(const SPHParameters& parameters, float dt)
    {
        AIKO_FUNCTION_PROFILE

        for (
            size_t i = 0;
            i < m_particles.size();
            ++i)
        {
            const aiko::vector<size_t> neighbours =
                m_hashGrid
                    .getNeighbourOfParticlesIdx(
                        i);

            SPHParticle& particleA =
                m_particles[i];

            for (
                size_t neighbourIndex :
                neighbours)
            {
                if (i == neighbourIndex)
                {
                    continue;
                }

                SPHParticle& particleB =
                    m_particles[
                        neighbourIndex];

                const aiko::u64 particleAIndex =
                    static_cast<aiko::u64>(
                        std::min(
                            i,
                            neighbourIndex));

                const aiko::u64 particleBIndex =
                    static_cast<aiko::u64>(
                        std::max(
                            i,
                            neighbourIndex));

                const aiko::u64 springId =
                    (particleAIndex << 32) |
                    particleBIndex;

                if (
                    m_springs.contains(
                        springId))
                {
                    continue;
                }

                const aiko::vec3 direction =
                    particleB.position -
                    particleA.position;

                const float distance =
                    aiko::math::length(
                        direction);

                const float q =
                    distance /
                    parameters.smoothingRadius;

                if (q >= 1.0f)
                {
                    continue;
                }

                const Spring spring
                {
                    .particleA =
                        static_cast<size_t>(
                            particleAIndex),

                    .particleB =
                        static_cast<size_t>(
                            particleBIndex),

                    .length =
                        parameters.smoothingRadius
                };

                m_springs.emplace(
                    springId,
                    spring);
            }
        }

        aiko::vector<aiko::u64>
            springsToErase;

        for (
            auto& [key, spring] :
            m_springs)
        {
            const SPHParticle& particleA =
                m_particles[
                    spring.particleA];

            const SPHParticle& particleB =
                m_particles[
                    spring.particleB];

            const aiko::vec3 direction =
                particleA.position -
                particleB.position;

            const float distance =
                aiko::math::length(
                    direction);

            const float deformation =
                parameters.gamma *
                spring.length;

            if (
                distance >
                spring.length +
                    deformation)
            {
                spring.length +=
                    dt *
                    parameters.plasticity *
                    (
                        distance -
                        spring.length -
                        deformation
                    );
            }
            else if (
                distance <
                spring.length -
                    deformation)
            {
                spring.length -=
                    dt *
                    parameters.plasticity *
                    (
                        spring.length -
                        deformation -
                        distance
                    );
            }

            if (
                spring.length >
                parameters.smoothingRadius)
            {
                springsToErase.emplace_back(
                    key);
            }
        }

        for (
            aiko::u64 springId :
            springsToErase)
        {
            m_springs.erase(
                springId);
        }
    }

    void SPHCpuSimulation::springDisplacement(
        const SPHParameters& parameters,
        float dt)
    {
        AIKO_FUNCTION_PROFILE

        for (
            auto& [key, spring] :
            m_springs)
        {
            (void)key;

            SPHParticle& particleA =
                m_particles[
                    spring.particleA];

            SPHParticle& particleB =
                m_particles[
                    spring.particleB];

            const aiko::vec3 direction =
                particleB.position -
                particleA.position;

            const float distance =
                aiko::math::length(
                    direction);

            if (distance <= 1e-6f)
            {
                continue;
            }

            const aiko::vec3 normalizedDirection =
                direction /
                distance;

            const float displacementTerm =
                dt *
                dt *
                parameters.springStiffness *
                (
                    1.0f -
                    spring.length /
                        parameters.smoothingRadius
                ) *
                (
                    spring.length -
                    distance
                );

            const aiko::vec3 displacement =
                normalizedDirection *
                displacementTerm *
                0.5f;

            particleA.position -=
                displacement;

            particleB.position +=
                displacement;
        }
    }

    void SPHCpuSimulation::doubleDensityRelaxation(
        const SPHParameters& parameters,
        float dt)
    {
        AIKO_FUNCTION_PROFILE

        for (
            size_t i = 0;
            i < m_particles.size();
            ++i)
        {
            float density = 0.0f;
            float densityNear = 0.0f;

            const aiko::vector<size_t> neighbours =
                m_hashGrid
                    .getNeighbourOfParticlesIdx(
                        i);

            SPHParticle& particleA =
                m_particles[i];

            for (
                size_t neighbourIndex :
                neighbours)
            {
                if (i == neighbourIndex)
                {
                    continue;
                }

                const SPHParticle& particleB =
                    m_particles[
                        neighbourIndex];

                const aiko::vec3 direction =
                    particleB.position -
                    particleA.position;

                const float distance =
                    aiko::math::length(
                        direction);

                const float q =
                    distance /
                    parameters.smoothingRadius;

                if (q >= 1.0f)
                {
                    continue;
                }

                const float oneMinusQ =
                    1.0f -
                    q;

                density +=
                    oneMinusQ *
                    oneMinusQ;

                densityNear +=
                    oneMinusQ *
                    oneMinusQ *
                    oneMinusQ;
            }

            const float pressure =
                parameters.pressureStiffness *
                (
                    density -
                    parameters.restDensity
                );

            const float pressureNear =
                parameters.nearPressureStiffness *
                densityNear;

            aiko::vec3 particleADisplacement =
            {
                0.0f,
                0.0f,
                0.0f
            };

            for (
                size_t neighbourIndex :
                neighbours)
            {
                if (i == neighbourIndex)
                {
                    continue;
                }

                SPHParticle& particleB =
                    m_particles[
                        neighbourIndex];

                const aiko::vec3 direction =
                    particleB.position -
                    particleA.position;

                const float distance =
                    aiko::math::length(
                        direction);

                const float q =
                    distance /
                    parameters.smoothingRadius;

                if (
                    q >= 1.0f ||
                    distance <= 1e-6f)
                {
                    continue;
                }

                const aiko::vec3 normalizedDirection =
                    direction /
                    distance;

                const float oneMinusQ =
                    1.0f -
                    q;

                const float displacementTerm =
                    dt *
                    dt *
                    (
                        pressure *
                            oneMinusQ +
                        pressureNear *
                            oneMinusQ *
                            oneMinusQ
                    );

                const aiko::vec3 displacement =
                    normalizedDirection *
                    displacementTerm;

                particleB.position +=
                    displacement *
                    0.5f;

                particleADisplacement -=
                    displacement *
                    0.5f;
            }

            particleA.position +=
                particleADisplacement;
        }
    }

    void SPHCpuSimulation::applyGravity(
        const SPHParameters& parameters,
        float dt)
    {
        AIKO_FUNCTION_PROFILE

        for (
            SPHParticle& particle :
            m_particles)
        {
            particle.velocity +=
                parameters.gravityDirection *
                (
                    parameters.gravity *
                    dt
                );
        }
    }

    void SPHCpuSimulation::handleOneWayCoupling(
        const SPHParameters& parameters,
        const aiko::vector<Shape>& shapes)
    {
        AIKO_FUNCTION_PROFILE

        for (
            SPHParticle& particle :
            m_particles)
        {
            for (
                const Shape& shape :
                shapes)
            {
                aiko::vec3 directionOut =
                {
                    0.0f,
                    0.0f,
                    0.0f
                };

                if (
                    shape.getDirectionOut(
                        particle.position,
                        parameters.particleRadius,
                        directionOut))
                {
                    particle.position +=
                        directionOut;
                }
            }
        }
    }

    void SPHCpuSimulation::handleStickiness(
        const SPHParameters& parameters,
        const aiko::vector<Shape>& shapes,
        float dt)
    {
        AIKO_FUNCTION_PROFILE

        if (
            parameters.maxStickiness <=
            0.0f)
        {
            return;
        }

        for (
            SPHParticle& particle :
            m_particles)
        {
            for (
                const Shape& shape :
                shapes)
            {
                aiko::vec3 nearestVector =
                {
                    0.0f,
                    0.0f,
                    0.0f
                };

                if (
                    !shape.getNearestVector(
                        particle.position,
                        parameters.maxStickiness,
                        nearestVector))
                {
                    continue;
                }

                const float distance =
                    aiko::math::length(
                        nearestVector);

                if (distance <= 1e-6f)
                {
                    continue;
                }

                const aiko::vec3 direction =
                    nearestVector /
                    distance;

                const float stickyTerm =
                    dt *
                    parameters.kStick *
                    distance *
                    (
                        1.0f -
                        distance /
                            parameters.maxStickiness
                    );

                particle.position +=
                    direction *
                    stickyTerm;
            }
        }
    }

    void SPHCpuSimulation::worldBoundary(
        const SPHParameters& parameters,
        const WorldBounds& bounds)
    {
        AIKO_FUNCTION_PROFILE

        const aiko::vec3 halfSize =
            bounds.size *
            0.5f;

        const float radius =
            parameters.particleRadius;

        const float left =
            bounds.position.x -
            halfSize.x +
            radius;

        const float right =
            bounds.position.x +
            halfSize.x -
            radius;

        const float bottom =
            bounds.position.y -
            halfSize.y +
            radius;

        const float top =
            bounds.position.y +
            halfSize.y -
            radius;

        for (
            SPHParticle& particle :
            m_particles)
        {
            if (particle.position.x < left)
            {
                particle.position.x =
                    left;

                particle.prevPosition.x =
                    left;
            }

            if (particle.position.x > right)
            {
                particle.position.x =
                    right;

                particle.prevPosition.x =
                    right;
            }

            if (particle.position.y < bottom)
            {
                particle.position.y =
                    bottom;

                particle.prevPosition.y =
                    bottom;
            }

            if (particle.position.y > top)
            {
                particle.position.y =
                    top;

                particle.prevPosition.y =
                    top;
            }
        }
    }

    void SPHCpuSimulation::neighboursSearch(
        const aiko::vec3& mousePosition,
        float smoothingRadius)
    {
        AIKO_FUNCTION_PROFILE

        if (m_particles.empty())
        {
            return;
        }

        m_hashGrid.clearGrid();
        m_hashGrid.mapParticlesToCell();

        size_t closestParticleIndex = 0;

        float closestDistance =
            std::numeric_limits<float>::max();

        for (
            size_t i = 0;
            i < m_particles.size();
            ++i)
        {
            SPHParticle& particle =
                m_particles[i];

            particle.color =
                aiko::BLUE;

            const float distance =
                aiko::math::lengthSquared(
                    particle.position -
                    mousePosition);

            if (distance < closestDistance)
            {
                closestDistance =
                    distance;

                closestParticleIndex =
                    i;
            }
        }

        const aiko::vector<size_t> neighbours =
            m_hashGrid
                .getNeighbourOfParticlesIdx(
                    closestParticleIndex);

        const SPHParticle& selected =
            m_particles[
                closestParticleIndex];

        const float smoothingRadiusSquared =
            smoothingRadius *
            smoothingRadius;

        for (
            size_t particleIndex :
            neighbours)
        {
            SPHParticle& particle =
                m_particles[
                    particleIndex];

            const aiko::vec3 direction =
                particle.position -
                selected.position;

            if (
                aiko::math::lengthSquared(
                    direction) <
                smoothingRadiusSquared)
            {
                particle.color =
                    aiko::YELLOW;
            }
        }

        m_particles[
            closestParticleIndex]
            .color =
                aiko::RED;
    }

}
