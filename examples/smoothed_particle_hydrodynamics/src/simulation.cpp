#include "simulation.h"

#include "intrumentor/profiler.h"
#include "math/math.h"
#include "models/mesh_factory.h"

#include <core/random.h>
#include <time/time.h>

#include <limits>

namespace sph
{

    void Simulation::init()
    {
        AIKO_FUNCTION_PROFILE
        m_particles.reserve(N_PARTICLES + 1024);
        m_particles.resize(N_PARTICLES);

        constexpr int columns = 32;
        constexpr float spacing = 0.10f;

        const aiko::vec3 halfSize = m_bounds.size * 0.5f;
        const float radius = m_parameters.particleRadius;

        const float left = m_bounds.position.x - halfSize.x + radius + 0.25f;
        const float top = m_bounds.position.y + halfSize.y - radius - 0.25f;

        for (size_t i = 0; i < m_particles.size(); ++i)
        {
            const int x = static_cast<int>(i) % columns;
            const int y = static_cast<int>(i) / columns;

            SPHParticle& particle = m_particles[i];

            particle.position =
            {
                left + static_cast<float>(x) * spacing,
                top - static_cast<float>(y) * spacing,
                0.0f
            };

            particle.prevPosition = particle.position;

            particle.velocity =
            {
                0.0f,
                0.0f,
                0.0f
            };

            particle.color = aiko::BLUE;
        }

        m_hashGrid.init(this, m_parameters.smoothingRadius);

        const EmitterSettings emitter
        {
            .position = {0.0f, 2.0f, 0.0f},
            .direction = {0.0f, -1.0f, 0.0f},
            .size = 1.0f,
            .spawnInterval = 1.0f,
            .amount = 20,
            .velocity = 0.2f,
            .angularVelocity = 0.1f,
        };
        createParticleEmitter(emitter);

        m_shapes.emplace_back(aiko::vec3{0.0f, 0.0f, 0.0f}, aiko::mesh::factory::generateCircle(12), aiko::MAGENTA);
        m_shapes.emplace_back(aiko::vec3{0.25f, 0.25f, 0.0f}, aiko::mesh::factory::generateTriangle(), aiko::MAGENTA);

    }

    void Simulation::update()
    {
        AIKO_FUNCTION_PROFILE
        AIKO_PLOT("SPH Particles", static_cast<double>(m_particles.size()));
        AIKO_PLOT("SPH Springs", static_cast<double>(m_springs.size()));
        const float dt = m_parameters.fixedDeltaTime;
        if (dt <= 0.0f)
        {
            return;
        }

        aiko::vector<SPHParticle> spawnedParticles;
        updateEmitters(dt, spawnedParticles);
        m_particles.insert(m_particles.end(), spawnedParticles.begin(), spawnedParticles.end());

        applyGravity(dt);
        m_hashGrid.clearGrid();
        m_hashGrid.mapParticlesToCell();
        viscosity(dt);
        predictPositions(dt);
        adjustSpring(dt);
        springDisplacement(dt);
        doubleDensityRelaxation(dt);
        handleStickiness(dt);
        handleOneWayCoupling();
        worldBoundary();
        computeNextVelocity(dt);
    }

    void Simulation::updateEmitters(float dt, aiko::vector<SPHParticle>& spawnedParticles)
    {
        for (ParticleEmitter& emitter : m_emitters)
        {
            emitter.spawn(dt, spawnedParticles);
            emitter.rotate(dt);
        }
    }

    void Simulation::predictPositions(float dt)
    {
        AIKO_FUNCTION_PROFILE
        for (size_t i = 0 ; i < m_particles.size(); ++i)
        {
            SPHParticle& p = m_particles[i];
            p.prevPosition = p.position;
            p.position += p.velocity * (dt * VelocityDamping);
        }
    }

    void Simulation::computeNextVelocity(float dt)
    {
        AIKO_FUNCTION_PROFILE
        for (size_t i = 0 ; i < m_particles.size(); ++i)
        {
            SPHParticle& p = m_particles[i];
            aiko::vec3 direction = p.position - p.prevPosition;
            p.velocity = direction * ( 1.0f / dt );
        }
    }

    void Simulation::viscosity(float dt)
    {
        AIKO_FUNCTION_PROFILE

        for (size_t i = 0; i < m_particles.size(); ++i)
        {
            aiko::vector<size_t> neighbours;

            {
                AIKO_ZONE_NAMED("Viscosity Get Neighbours")
                neighbours = m_hashGrid.getNeighbourOfParticlesIdx(i);
            }

            SPHParticle& particleA = m_particles[i];

            {
                AIKO_ZONE_NAMED("Viscosity Solve")

                for (size_t j = 0; j < neighbours.size(); ++j)
                {
                    if (i == neighbours[j])
                    {
                        continue;
                    }

                    SPHParticle& particleB = m_particles[neighbours[j]];

                    const aiko::vec3 directionNeighbour = particleB.position - particleA.position;

                    const aiko::vec3 velocityA = particleA.velocity;
                    const aiko::vec3 velocityB = particleB.velocity;

                    const float distance = aiko::math::length(directionNeighbour);

                    const float q = distance / m_parameters.smoothingRadius;

                    if (q < 1.0f)
                    {
                        const aiko::vec3 normalizedDir = aiko::math::normalize(directionNeighbour);

                        const float u = aiko::math::dot(velocityA - velocityB, normalizedDir);

                        if (u > 0)
                        {
                            const float term = dt * (1.0f - q) * (m_parameters.sigma * u + m_parameters.beta * u * u);

                            const aiko::vec3 I = term * normalizedDir;

                            particleA.velocity -= I * 0.5f;
                            particleB.velocity += I * 0.5f;
                        }
                    }
                }
            }
        }
    }

    void Simulation::adjustSpring(float dt)
    {
        AIKO_FUNCTION_PROFILE

        for (size_t i = 0; i < m_particles.size(); ++i)
        {
            aiko::vector<size_t> neighbours;

            {
                AIKO_ZONE_NAMED("Spring Get Neighbours")
                neighbours = m_hashGrid.getNeighbourOfParticlesIdx(i);
            }

            SPHParticle& particleA = m_particles[i];

            {
                AIKO_ZONE_NAMED("Spring Create")

                for (size_t j = 0; j < neighbours.size(); ++j)
                {
                    if (i == neighbours[j])
                    {
                        continue;
                    }

                    SPHParticle& particleB = m_particles[neighbours[j]];

                    const size_t neighbourIdx = neighbours[j];

                    const aiko::u64 particleAIdx = static_cast<aiko::u64>(std::min(i, neighbourIdx));

                    const aiko::u64 particleBIdx = static_cast<aiko::u64>(std::max(i, neighbourIdx));

                    const aiko::u64 springId = (particleAIdx << 32) | particleBIdx;

                    if (m_springs.contains(springId))
                    {
                        continue;
                    }

                    const aiko::vec3 directionNeighbour = particleB.position - particleA.position;

                    const float distance = aiko::math::length(directionNeighbour);

                    const float q = distance / m_parameters.smoothingRadius;

                    if (q < 1.0f)
                    {
                        const Spring spring
                        {
                            .particleA = static_cast<size_t>(particleAIdx),
                            .particleB = static_cast<size_t>(particleBIdx),
                            .length = m_parameters.smoothingRadius
                        };

                        m_springs.emplace(springId, spring);
                    }
                }
            }
        }

        aiko::vector<aiko::u64> springsToErase;

        {
            AIKO_ZONE_NAMED("Spring Plasticity")

            for (auto& [key, spring] : m_springs)
            {
                const SPHParticle& particleA = m_particles[spring.particleA];
                const SPHParticle& particleB = m_particles[spring.particleB];

                const aiko::vec3 direction = particleA.position - particleB.position;
                const float distance = aiko::math::length(direction);
                const float deformation = m_parameters.gamma * spring.length;

                if (distance > spring.length + deformation)
                {
                    spring.length += dt * m_parameters.plasticity * (distance - spring.length - deformation);
                }
                else if (distance < spring.length - deformation)
                {
                    spring.length -= dt * m_parameters.plasticity * (spring.length - deformation - distance);
                }

                if (spring.length > m_parameters.smoothingRadius)
                {
                    springsToErase.emplace_back(key);
                }
            }
        }

        {
            AIKO_ZONE_NAMED("Spring Erase")

            for (const aiko::u64 springId : springsToErase)
            {
                m_springs.erase(springId);
            }
        }
    }

    void Simulation::springDisplacement(float dt)
    {
        AIKO_FUNCTION_PROFILE
        for (auto& [key, spring] : m_springs)
        {
            SPHParticle& particleA = m_particles[spring.particleA];
            SPHParticle& particleB = m_particles[spring.particleB];

            const aiko::vec3 direction = particleB.position - particleA.position;
            const float distance = aiko::math::length(direction);

            if (distance <= 1e-6f)
            {
                continue;
            }

            const aiko::vec3 normalizedDirection = direction / distance;
            const float displacementTerm = dt * dt * m_parameters.springStiffness * (1.0f - spring.length / m_parameters.smoothingRadius) * (spring.length - distance);
            const aiko::vec3 displacement = normalizedDirection * displacementTerm * 0.5f;

            particleA.position -= displacement;
            particleB.position += displacement;
        }
    }

    void Simulation::doubleDensityRelaxation(float dt)
    {
        AIKO_FUNCTION_PROFILE

        for (size_t i = 0; i < m_particles.size(); ++i)
        {
            float density = 0.0f;
            float densityNear = 0.0f;

            aiko::vector<size_t> neighbours;

            {
                AIKO_ZONE_NAMED("DDR Get Neighbours")
                neighbours = m_hashGrid.getNeighbourOfParticlesIdx(i);
            }

            SPHParticle& particleA = m_particles[i];

            {
                AIKO_ZONE_NAMED("DDR Density")

                for (size_t j = 0; j < neighbours.size(); ++j)
                {
                    if (i == neighbours[j])
                    {
                        continue;
                    }

                    const SPHParticle& particleB = m_particles[neighbours[j]];

                    const aiko::vec3 directionNeighbour = particleB.position - particleA.position;

                    const float distance = aiko::math::length(directionNeighbour);

                    const float q = distance / m_parameters.smoothingRadius;

                    if (q < 1.0f)
                    {
                        density += aiko::math::pow(1.0f - q, 2);
                        densityNear += aiko::math::pow(1.0f - q, 3);
                    }
                }
            }

            const float pressure = m_parameters.pressureStiffness * (density - m_parameters.restDensity);

            const float pressureNear = m_parameters.nearPressureStiffness * densityNear;

            aiko::vec3 particleADisplacement{0.0f};

            {
                AIKO_ZONE_NAMED("DDR Displacement")

                for (size_t j = 0; j < neighbours.size(); ++j)
                {
                    if (i == neighbours[j])
                    {
                        continue;
                    }

                    SPHParticle& particleB = m_particles[neighbours[j]];

                    const aiko::vec3 directionNeighbour = particleB.position - particleA.position;

                    const float distance = aiko::math::length(directionNeighbour);

                    const float q = distance / m_parameters.smoothingRadius;

                    if (q < 1.0f && distance > 1e-6f)
                    {
                        const aiko::vec3 normalizedDirection = aiko::math::normalize(directionNeighbour);

                        const float displacementTerm = aiko::math::pow(dt, 2) * ( pressure * (1.0f - q) + pressureNear * aiko::math::pow(1.0f - q, 2) );

                        const aiko::vec3 displacement = normalizedDirection * displacementTerm;

                        particleB.position += displacement * 0.5f;

                        particleADisplacement -= displacement * 0.5f;
                    }
                }
            }

            particleA.position += particleADisplacement;
        }
    }

    void Simulation::applyGravity(float dt)
    {
        AIKO_FUNCTION_PROFILE
        for (size_t i = 0 ; i < m_particles.size(); ++i)
        {
            SPHParticle& p = m_particles[i];
            p.velocity += m_parameters.gravityDirection * (m_parameters.gravity * dt);
        }
    }

    void Simulation::handleOneWayCoupling()
    {
        AIKO_FUNCTION_PROFILE
        for (SPHParticle& particle: m_particles)
        {
            for (const Shape& shape: m_shapes)
            {
                aiko::vec3 directionOut = {};
                if (shape.getDirectionOut(particle.position, m_parameters.particleRadius, directionOut))
                {
                    particle.position += directionOut;
                }
            }
        }
    }

    void Simulation::handleStickiness(float dt)
    {
        AIKO_FUNCTION_PROFILE
        for (SPHParticle& particle : m_particles)
        {
            for (const Shape& shape : m_shapes)
            {
                aiko::vec3 nearestVector = {};

                if (!shape.getNearestVector(particle.position, m_parameters.maxStickiness, nearestVector))
                {
                    continue;
                }

                const float distance = aiko::math::length(nearestVector);

                if (distance <= 1e-6f)
                {
                    continue;
                }

                const aiko::vec3 direction = nearestVector / distance;

                const float stickyTerm = dt * m_parameters.kStick * distance * (1.0f - distance / m_parameters.maxStickiness);

                particle.position += direction * stickyTerm;
            }
        }
    }

    void Simulation::neighboursSearch(const aiko::vec3& mousePosition)
    {
        AIKO_FUNCTION_PROFILE
        m_hashGrid.clearGrid();
        m_hashGrid.mapParticlesToCell();

        size_t closestParticleIdx = 0;
        float closestDistance = std::numeric_limits<float>::max();

        for (size_t i = 0; i < m_particles.size(); ++i)
        {
            SPHParticle& particle = m_particles[i];
            particle.color = aiko::BLUE;

            const float distance = aiko::math::lengthSquared(particle.position - mousePosition);

            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestParticleIdx = i;
            }
        }

        const auto neighbours = m_hashGrid.getNeighbourOfParticlesIdx(closestParticleIdx);

        const SPHParticle& selected = m_particles[closestParticleIdx];

        for (const size_t particleIndex : neighbours)
        {
            SPHParticle& particle = m_particles[particleIndex];
            const aiko::vec3 direction = particle.position - selected.position;
            const float distanceSquared = aiko::math::lengthSquared(direction);
            const float smoothingRadiusSquared = m_parameters.smoothingRadius * m_parameters.smoothingRadius;
            if (distanceSquared < smoothingRadiusSquared)
            {
                particle.color = aiko::YELLOW;
            }
        }
        m_particles[closestParticleIdx].color = aiko::RED;

    }
    ParticleEmitter* Simulation::createParticleEmitter(const EmitterSettings settings)
    {
        ParticleEmitter& emitter = m_emitters.emplace_back();
        emitter.init(settings);
        return &emitter;
    }

    void Simulation::worldBoundary()
    {
        AIKO_FUNCTION_PROFILE
        const aiko::vec3 halfSize = m_bounds.size * 0.5f;

        const float radius = m_parameters.particleRadius;
        const float left = m_bounds.position.x - halfSize.x + radius;
        const float right = m_bounds.position.x + halfSize.x - radius;
        const float bottom = m_bounds.position.y - halfSize.y + radius;
        const float top = m_bounds.position.y + halfSize.y - radius;

        for (SPHParticle& particle : m_particles)
        {
            if (particle.position.x < left)
            {
                particle.position.x = left;
                particle.prevPosition.x = left;
            }

            if (particle.position.x > right)
            {
                particle.position.x = right;
                particle.prevPosition.x = right;
            }

            if (particle.position.y < bottom)
            {
                particle.position.y = bottom;
                particle.prevPosition.y = bottom;
            }

            if (particle.position.y > top)
            {
                particle.position.y = top;
                particle.prevPosition.y = top;
            }
        }
    }
}
