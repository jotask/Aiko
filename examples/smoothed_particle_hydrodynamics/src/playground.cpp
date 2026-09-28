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

        const auto& shapes = m_simulation.shapes();
        m_shapeRenderData.resize(shapes.size());
        for (size_t i = 0; i < shapes.size(); ++i)
        {
            const Shape& shape = shapes[i];
            ShapeRenderData& renderData = m_shapeRenderData[i];
            renderData.mesh.upload(shape.asset());
            renderData.material.m_shaderId = shaderId;
            renderData.material.m_useVertexColor = false;
            renderData.material.m_lit = false;
            renderData.material.m_baseColor = shape.color();
        }

        m_particleMesh.upload(aiko::mesh::factory::generateCircle(12));

        m_particleMaterial.m_shaderId = shaderId;
        m_particleMaterial.m_useVertexColor = true;
        m_particleMaterial.m_lit = false;
        m_particleMaterial.m_baseColor = aiko::WHITE;
    }

    void Playground::update(const aiko::InputContext& input, const aiko::vec3& mousePosition)
    {
        m_simulation.update();
        m_simulation.neighboursSearch(mousePosition);

        if (input.isMouseButtonJustPressed(aiko::MouseButton::MOUSE_BUTTON_LEFT))
        {
            const aiko::vector<Shape>& shapes = m_simulation.shapes();
            m_selectedShape = std::nullopt;
            for (size_t i = 0; i < shapes.size(); ++i)
            {
                if (shapes[i].isPointInside(mousePosition))
                {
                    m_selectedShape = i;
                    break;
                }
            }
            m_previousMousePosition = mousePosition;
        }

        if (input.isMouseButtonPressed(aiko::MouseButton::MOUSE_BUTTON_LEFT) && m_selectedShape.has_value())
        {
            aiko::vector<Shape>& shapes = m_simulation.shapes();
            Shape& shape = shapes[m_selectedShape.value()];
            const aiko::vec3 offset = mousePosition - m_previousMousePosition;
            shape.moveBy(offset);
            m_previousMousePosition = mousePosition;
        }

        if (input.isMouseButtonJustReleased(aiko::MouseButton::MOUSE_BUTTON_LEFT))
        {
            m_selectedShape = std::nullopt;
        }
    }

    void Playground::render(aiko::RenderContext& renderer, aiko::RenderSystem& renderSystem)
    {

        // Particles
        const auto& particles = m_simulation.particles();

        m_particleInstances.resize(particles.size());

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

        // Emitters
        const auto& emitters = m_simulation.emitters();
        for (size_t i = 0; i < emitters.size(); ++i)
        {
            const ParticleEmitter& emitter = emitters[i];
            const EmitterSettings& settings = emitter.settings();
            const aiko::vec3 direction = settings.direction;
            const aiko::vec3 normalizedDirection = aiko::math::normalize(direction);
            const aiko::vec3 normal = { -normalizedDirection.y, normalizedDirection.x, 0.0f };
            const aiko::vec3 halfPlane = normal * (settings.size * 0.5f);
            const aiko::vec3 planeStart = settings.position - halfPlane;
            const aiko::vec3 planeEnd = settings.position + halfPlane;
            renderSystem.renderLine(planeStart, planeEnd);
        }

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

        // Shapes
        const auto& shapes = m_simulation.shapes();

        for (size_t i = 0; i < shapes.size(); ++i)
        {
            const Shape& shape = shapes[i];
            const ShapeRenderData& renderData = m_shapeRenderData[i];

            aiko::Transform transform;
            transform.position = shape.position();

            renderer.drawMesh(transform, renderData.mesh, renderData.material);
        }
    }
}
