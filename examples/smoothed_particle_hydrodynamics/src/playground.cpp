#include "playground.h"

#include "assets/types/mesh_asset.h"
#include "layers/contexts/render_context.h"
#include "models/mesh_factory.h"
#include "systems/render_system.h"

namespace sph
{

    Playground::Playground() = default;
    Playground::~Playground() = default;

    void Playground::init(const aiko::AssetId& shaderId)
    {
        m_simulation.init();

        m_particleMesh.upload(aiko::mesh::factory::generateCircle(12));

        m_particleMaterial.m_shaderId = shaderId;
        m_particleMaterial.m_useVertexColor = true;
        m_particleMaterial.m_lit = false;
        m_particleMaterial.m_baseColor = aiko::WHITE;
    }

    void Playground::update()
    {
        m_simulation.update();
    }

    void Playground::render(aiko::RenderContext& renderer, aiko::RenderSystem& renderSystem)
    {

        // Particles
        const auto& particles = m_simulation.particles();

        const float diameter = m_simulation.parameters().particleRadius * 2.0f;

        for (size_t i = 0; i < particles.size(); ++i)
        {
            const SPHParticle& particle = particles[i];

            m_particleInstances[i] =
            {
                .position = particle.position,
                .rotation = {0.0f, 0.0f, 0.0f},
                .scale =
                {
                    diameter,
                    diameter,
                    diameter
                },
                .color = particle.color
            };
        }

        renderer.drawMeshInstanced(m_particleMesh, m_particleMaterial, m_particleInstances.data(), static_cast<aiko::u32>(m_particleInstances.size()));

        // Bounding box
        const WorldBounds& bounds = m_simulation.bounds();

        const aiko::vec3 halfSize = bounds.size * 0.5f;

        const aiko::vec3 bottomLeft =
        {
            bounds.position.x - halfSize.x,
            bounds.position.y - halfSize.y,
            0.0f
        };

        const aiko::vec3 bottomRight =
        {
            bounds.position.x + halfSize.x,
            bounds.position.y - halfSize.y,
            0.0f
        };

        const aiko::vec3 topLeft =
        {
            bounds.position.x - halfSize.x,
            bounds.position.y + halfSize.y,
            0.0f
        };

        const aiko::vec3 topRight =
        {
            bounds.position.x + halfSize.x,
            bounds.position.y + halfSize.y,
            0.0f
        };

        renderSystem.renderLine(bottomLeft, bottomRight);
        renderSystem.renderLine(bottomRight, topRight);
        renderSystem.renderLine(topRight, topLeft);
        renderSystem.renderLine(topLeft, bottomLeft);
    }

}
