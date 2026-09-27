#include "simulation.h"

#include <time/time.h>
#include <core/random.h>

namespace sph
{

    void Simulation::init()
    {
        constexpr int columns = 32;
        constexpr int rows = static_cast<int>(N_PARTICLES) / columns;

        const float radius = m_parameters.particleRadius;

        const aiko::vec3 halfSize = m_bounds.size * 0.5f;

        const float left = m_bounds.position.x - halfSize.x + radius;
        const float right = m_bounds.position.x + halfSize.x - radius;
        const float bottom = m_bounds.position.y - halfSize.y + radius;
        const float top = m_bounds.position.y + halfSize.y - radius;

        const float spacingX = (right - left) / static_cast<float>(columns - 1);
        const float spacingY = (top - bottom) / static_cast<float>(rows - 1);

        for (size_t i = 0; i < m_particles.size(); ++i)
        {
            const int x = static_cast<int>(i) % columns;
            const int y = static_cast<int>(i) / columns;

            SPHParticle& particle = m_particles[i];

            particle.position =
            {
                left + static_cast<float>(x) * spacingX,
                top - static_cast<float>(y) * spacingY,
                0.0f
            };

            particle.velocity =
            {
                aiko::utils::getRandomValue(-1.0f, 1.0f) * 0.25f,
                aiko::utils::getRandomValue(-1.0f, 1.0f) * 0.25f,
                0.0f
            };

            particle.prevPosition = particle.position;
            particle.color = aiko::BLUE;

        }

        m_hashGrid.init(this, m_parameters.smoothingRadius);

    }

    void Simulation::update()
    {
        const float dt = aiko::Time::it().getDeltaTime();
        if (dt <= 0.0f)
        {
            return;
        }
        predictPositions(dt);
        computeNextVelocity(dt);

        worldBoundary();
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

    void Simulation::neighboursSearch(const aiko::vec3& mousePosition)
    {
        m_hashGrid.clearGrid();
        m_hashGrid.mapParticlesToCell();

        auto gridHashGrid = m_hashGrid.getGridHashFromPosition(mousePosition);
        auto* contentOffCell = m_hashGrid.getContentOfCell(gridHashGrid);

        for (SPHParticle& particle : m_particles)
        {
            particle.color = aiko::BLUE;
        }

        if (contentOffCell != nullptr)
        {
            for (const size_t particleIndex : *contentOffCell)
            {
                m_particles[particleIndex].color = aiko::YELLOW;
            }
        }

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
                particle.velocity.x *= -BoundaryDamping;
            }
            else if (particle.position.x > right)
            {
                particle.position.x = right;
                particle.velocity.x *= -BoundaryDamping;
            }

            if (particle.position.y < bottom)
            {
                particle.position.y = bottom;
                particle.velocity.y *= -BoundaryDamping;
            }
            else if (particle.position.y > top)
            {
                particle.position.y = top;
                particle.velocity.y *= -BoundaryDamping;
            }
        }
    }
}
