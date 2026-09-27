#include "simulation.h"

#include "time/time.h"

namespace sph
{

    void Simulation::init()
    {

        constexpr int columns = 32;
        constexpr float spacing = 0.12f;

        for (size_t i = 0; i < m_particles.size(); ++i)
        {
            const int x = static_cast<int>(i) % columns;
            const int y = static_cast<int>(i) / columns;

            SPHParticle& particle = m_particles[i];

            particle.position =
            {
                -2.0f + static_cast<float>(x) * spacing,
                2.0f - static_cast<float>(y) * spacing,
                0.0f
            };

            particle.prevPosition = particle.position;
            particle.color = aiko::BLUE;
        }
    }

    void Simulation::update()
    {

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
}
