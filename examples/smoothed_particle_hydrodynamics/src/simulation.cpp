#include "simulation.h"

#include "math/math.h"

#include <core/random.h>
#include <time/time.h>

#include <limits>

namespace sph
{

    void Simulation::init()
    {

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

    }

    void Simulation::update()
    {
        const float dt = m_parameters.fixedDeltaTime;
        if (dt <= 0.0f)
        {
            return;
        }

        for (ParticleEmitter& emitter : m_emitters)
        {
            emitter.spawn(dt);
            emitter.rotate(dt);
        }

        applyGravity(dt);
        m_hashGrid.clearGrid();
        m_hashGrid.mapParticlesToCell();
        viscosity(dt);
        predictPositions(dt);
        doubleDensityRelaxation(dt);
        worldBoundary();
        computeNextVelocity(dt);
    }

    void Simulation::predictPositions(float dt)
    {
        for (size_t i = 0 ; i < m_particles.size(); ++i)
        {
            SPHParticle& p = m_particles[i];
            p.prevPosition = p.position;
            p.position += p.velocity * (dt * VelocityDamping);
        }
    }

    void Simulation::computeNextVelocity(float dt)
    {
        for (size_t i = 0 ; i < m_particles.size(); ++i)
        {
            SPHParticle& p = m_particles[i];
            aiko::vec3 direction = p.position - p.prevPosition;
            p.velocity = direction * ( 1.0f / dt );
        }
    }

    void Simulation::viscosity(float dt)
    {
        for (size_t i = 0 ; i < m_particles.size(); ++i)
        {
            aiko::vector<size_t> neighbours = m_hashGrid.getNeighbourOfParticlesIdx(i);
            SPHParticle& particleA = m_particles[i];
            for (size_t j = 0 ; j < neighbours.size(); ++j)
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
                        auto term = dt * ( 1 - q) * ( m_parameters.sigma * u + m_parameters.beta * u * u );
                        auto I = term * normalizedDir;

                        particleA.velocity -= I * 0.5f;
                        particleB.velocity += I * 0.5f;
                    }
                }

            }
        }
    }

    void Simulation::doubleDensityRelaxation(float dt)
    {
        for (size_t i = 0 ; i < m_particles.size(); ++i)
        {
            float density = 0.0f;
            float densityNear = 0.0f;
            aiko::vector<size_t> neighbours = m_hashGrid.getNeighbourOfParticlesIdx(i);
            SPHParticle& particleA = m_particles[i];
            for (size_t j = 0 ; j < neighbours.size(); ++j)
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
                    density += aiko::math::pow( 1.0f - q, 2);
                    densityNear += aiko::math::pow( 1.0f - q, 3);
                }
            }

            const float pressure = m_parameters.pressureStiffness * (density - m_parameters.restDensity);
            const float pressureNear = m_parameters.nearPressureStiffness * densityNear;

            aiko::vec3 particleADisplacement{0.0f};

            for (size_t j = 0 ; j < neighbours.size(); ++j)
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
                    const float displacementTerm = aiko::math::pow(dt, 2) * ( pressure * (1 - q) + pressureNear * aiko::math::pow(1 - q, 2));
                    const aiko::vec3 displacement = normalizedDirection * displacementTerm;
                    particleB.position += displacement * 0.5f;
                    particleADisplacement -= displacement * 0.5f;
                }
            }

            particleA.position += particleADisplacement;
        }
    }

    void Simulation::applyGravity(float dt)
    {
        for (size_t i = 0 ; i < m_particles.size(); ++i)
        {
            SPHParticle& p = m_particles[i];
            p.velocity += m_parameters.gravityDirection * (m_parameters.gravity * dt);
        }
    }

    void Simulation::neighboursSearch(const aiko::vec3& mousePosition)
    {
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
        emitter.init(this, settings);
        return &emitter;
    }

    void Simulation::worldBoundary()
    {
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
