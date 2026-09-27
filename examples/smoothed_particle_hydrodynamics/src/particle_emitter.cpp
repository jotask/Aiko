#include "particle_emitter.h"

#include "math/math_transform.h"
#include "simulation.h"
#include "sph_types.h"

namespace sph
{

    void ParticleEmitter::init(Simulation* simulation, const EmitterSettings settings)
    {
        AIKO_ASSERT(simulation != nullptr, "Particle Emitter needs a valid simulation");
        m_simulation = simulation;
        m_settings = settings;
    }

    void ParticleEmitter::update()
    {

    }

    void ParticleEmitter::spawn(float dt)
    {
        const float spacing = m_settings.amount > 1 ? m_settings.size / static_cast<float>(m_settings.amount - 1) : 0.0f;

        m_time += dt;

        if (m_time > m_settings.spawnInterval)
        {
            m_time = 0.0f;

            for (size_t i = 0; i < m_settings.amount; ++i)
            {
                const aiko::vec3 normalizedDirection = aiko::math::normalize(m_settings.direction);

                const aiko::vec3 normal =
                {
                    -normalizedDirection.y,
                    normalizedDirection.x,
                    0.0f
                };

                const aiko::vec3 halfPlane = normal * (m_settings.size * 0.5f);

                const aiko::vec3 planeStart = m_settings.position - halfPlane;

                const aiko::vec3 position = planeStart + normal * (spacing * static_cast<float>(i));

                const SPHParticle particle
                {
                    .position = position,
                    .prevPosition = position,
                    .velocity = normalizedDirection * m_settings.velocity
                };

                m_simulation->particles().emplace_back(particle);
            }
        }
    }

    void ParticleEmitter::rotate(float dt)
    {
        const float angle = m_settings.angularVelocity * dt;

        const float cosAngle = aiko::math::cos(angle);
        const float sinAngle = aiko::math::sin(angle);

        const float rotateX = m_settings.direction.x * cosAngle - m_settings.direction.y * sinAngle;
        const float rotateY = m_settings.direction.x * sinAngle + m_settings.direction.y * cosAngle;

        m_settings.direction.x = rotateX;
        m_settings.direction.y = rotateY;
    }

    void ParticleEmitter::move(float dt)
    {
        m_settings.position += dt;
    }
}
