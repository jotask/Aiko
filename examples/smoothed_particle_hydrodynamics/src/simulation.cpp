#include "simulation.h"

#include "intrumentor/profiler.h"
#include "models/mesh_factory.h"

#include <utility>
#include <cmath>

namespace sph
{

    void Simulation::init(size_t particleCount)
    {
        AIKO_FUNCTION_PROFILE

        m_initialParticles.clear();
        m_initialParticles.resize(particleCount);

        const size_t columns = static_cast<size_t>(std::ceil(std::sqrt(static_cast<float>(particleCount))));
        constexpr float spacing = 0.10f;

        const aiko::vec3 halfSize = m_bounds.size * 0.5f;

        const float radius = m_parameters.particleRadius;

        const float left = m_bounds.position.x - halfSize.x + radius + 0.25f;

        const float top = m_bounds.position.y + halfSize.y - radius - 0.25f;

        for (size_t i = 0; i < m_initialParticles.size(); ++i)
        {
            const int x = static_cast<int>(i) % columns;

            const int y = static_cast<int>(i) / columns;

            SPHParticle& particle = m_initialParticles[i];

            particle.position =
            {
                left + static_cast<float>(x) * spacing,
                top - static_cast<float>(y) * spacing,
                0.0f
            };

            particle.prevPosition = particle.position;

            particle.velocity = { 0.0f, 0.0f, 0.0f };

            particle.color = aiko::BLUE;
        }

        const EmitterSettings emitter
        {
            .position = { 0.0f, 2.0f, 0.0f},
            .direction = { 0.0f, -1.0f, 0.0f},
            .size = 1.0f,
            .spawnInterval = 1.0f,
            .amount = 20,
            .velocity = 0.2f,
            .angularVelocity = 0.1f,
        };

        createParticleEmitter(emitter);

        createShape({ 0.0f, 0.0f, 0.0f }, aiko::mesh::factory::generateCircle(12), aiko::ORANGE);

        createShape( { 0.25f, 0.25f, 0.0f }, aiko::mesh::factory::generateTriangle(), aiko::ORANGE);
    }

    void Simulation::updateEmitters(float dt, aiko::vector<SPHParticle>& spawnedParticles)
    {
        for (ParticleEmitter& emitter : m_emitters)
        {
            emitter.spawn(dt, spawnedParticles);
            emitter.rotate(dt);
        }
    }

    ParticleEmitter* Simulation::createParticleEmitter(EmitterSettings settings)
    {
        ParticleEmitter& emitter = m_emitters.emplace_back();
        emitter.init(settings);
        return &emitter;
    }

    void Simulation::removeParticleEmitter(size_t index)
    {
        AIKO_ASSERT(index < m_emitters.size(), "Particle emitter index out of range");
        m_emitters.erase(m_emitters.begin() + static_cast<std::ptrdiff_t>(index));
    }

    Shape* Simulation::createShape(aiko::vec3 position, aiko::MeshAsset asset, aiko::Color color)
    {
        Shape& shape = m_shapes.emplace_back(position, std::move(asset), color);
        return &shape;
    }

    void Simulation::removeShape(size_t index)
    {
        AIKO_ASSERT(index < m_shapes.size(), "Shape index out of range");
        m_shapes.erase(m_shapes.begin() + static_cast<std::ptrdiff_t>(index));
    }

}
