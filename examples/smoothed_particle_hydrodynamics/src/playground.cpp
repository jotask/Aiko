#include "playground.h"

#include "assets/types/mesh_asset.h"
#include "layers/contexts/render_context.h"
#include "models/mesh_factory.h"

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

    void Playground::render(aiko::RenderContext& renderer)
    {
        const auto& particles = m_simulation.particles();

        constexpr float diameter = 0.1f;

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
    }

}
