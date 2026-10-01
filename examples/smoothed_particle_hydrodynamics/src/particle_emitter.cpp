#include "particle_emitter.h"

#include "math/math_transform.h"
#include "sph_types.h"

namespace sph
{

    void ParticleEmitter::init(const EmitterSettings settings)
    {
        m_settings = settings;
    }

    void ParticleEmitter::spawn(float dt, aiko::vector<SPHParticle>& spawnedParticles)
    {
        if (m_settings.spawnInterval <= 0.0f)
        {
            return;
        }

        const float directionLengthSquared = aiko::math::dot(m_settings.direction, m_settings.direction);

        if (directionLengthSquared <= 1e-6f)
        {
            return;
        }

        const aiko::vec3 normalizedDirection = aiko::math::normalize(m_settings.direction);

        const aiko::vec3 normal = { -normalizedDirection.y, normalizedDirection.x, 0.0f};

        const float spacing = m_settings.amount > 1 ? m_settings.size / static_cast<float>(m_settings.amount - 1) : 0.0f;

        const aiko::vec3 halfPlane = normal * (m_settings.size * 0.5f);

        const aiko::vec3 planeStart = m_settings.position - halfPlane;

        m_time += dt;

        while (m_time >= m_settings.spawnInterval)
        {
            m_time -= m_settings.spawnInterval;

            for ( size_t i = 0; i < m_settings.amount; ++i)
            {
                const aiko::vec3 position = planeStart + normal * ( spacing * static_cast<float>(i) );

                const SPHParticle particle
                {
                    .position = position,
                    .prevPosition = position,
                    .velocity = normalizedDirection * m_settings.velocity
                };

                spawnedParticles.emplace_back(particle);
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

}
