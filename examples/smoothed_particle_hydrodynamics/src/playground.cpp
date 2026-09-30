#include "playground.h"

#include "assets/types/mesh_asset.h"
#include "intrumentor/profiler.h"
#include "layers/contexts/render_context.h"
#include "models/mesh_factory.h"
#include "systems/render_system.h"
#include "systems/asset_system.h"

#include <magic_enum/magic_enum.hpp>
#include <imgui.h>

namespace sph
{

    namespace
    {
        constexpr bool EnableGpuSimulation = true;
    }

    Playground::Playground() = default;
    Playground::~Playground() = default;

    void Playground::init(const aiko::AssetId& shaderId, const aiko::AssetId& gpuShaderId, aiko::AssetSystem& assetSystem)
    {
        m_simulation.init();
        m_gpuSimulation.init(assetSystem, m_simulation.particles(), m_simulation.shapes());

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
        m_particleMaterial.m_baseColor = aiko::WHITE;
        m_particleMaterial.m_useVertexColor = true;
        m_particleMaterial.m_lit = false;

        m_gpuParticleMaterial.m_shaderId = gpuShaderId;
        m_gpuParticleMaterial.m_baseColor = aiko::WHITE;
        m_gpuParticleMaterial.m_useVertexColor = true;
        m_gpuParticleMaterial.m_lit = false;

        m_gpuParticleMaterial.setFloat("u_particleDiameter", m_simulation.parameters().particleRadius * 2.0f);
        m_gpuParticleMaterial.setFloat("u_particleDiameter", m_simulation.parameters().particleRadius * 2.0f);
        m_gpuParticleMaterial.setUInt("u_particleColorMode", static_cast<uint32_t>(m_particleColorMode));
        m_gpuParticleMaterial.setFloat("u_velocityColorScale", m_velocityColorScale);
        m_gpuParticleMaterial.setFloat("u_pressureColorScale", m_pressureColorScale);
        m_gpuParticleMaterial.setFloat("u_velocityColorScale", 0.5f);
        m_gpuParticleMaterial.setFloat("u_pressureColorScale", 0.05f);

    }

    void Playground::update(const aiko::InputContext& input, const aiko::vec3& mousePosition)
    {
        AIKO_FUNCTION_PROFILE

        if constexpr (EnableGpuSimulation == false)
        {
            m_simulation.update();
            m_simulation.neighboursSearch(mousePosition);
        }
        else
        {
            // aiko::vector<SPHParticle> spawnedParticles;
            // m_simulation.updateEmitters(m_simulation.parameters().fixedDeltaTime, spawnedParticles);
            // m_gpuSimulation.spawnParticles(spawnedParticles);
        }

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
        AIKO_FUNCTION_PROFILE
        // Particles

        if constexpr (EnableGpuSimulation == false)
        {
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
        }
        else
        {

            m_gpuSimulation.updateShapes(m_simulation.shapes());
            m_gpuSimulation.update(renderSystem, m_simulation.parameters(), m_simulation.bounds());

            aiko::GpuInstanceDrawDesc draw{};
            draw.mesh = &m_particleMesh;
            draw.material = &m_gpuParticleMaterial;
            draw.readBuffers.push_back({7, &m_gpuSimulation.positionBuffer()});
            draw.readBuffers.push_back({8, &m_gpuSimulation.velocityBuffer()});
            draw.readBuffers.push_back({9, &m_gpuSimulation.pressureBuffer()});
            draw.instanceCount = m_gpuSimulation.particleCount();

            m_gpuParticleMaterial.setUInt("u_particleColorMode",static_cast<uint32_t>(m_particleColorMode));

            renderer.drawMeshInstancedGpu(draw);
        }

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

        renderGui();

    }

    void Playground::renderGui()
    {
        if (ImGui::Begin("SPH Simulation"))
        {
            SPHParameters& parameters = m_simulation.parameters();

            ImGui::Text("Particles: %u", m_gpuSimulation.particleCount());

            ImGui::SeparatorText("Fluid");

            ImGui::DragFloat("Particle Radius", &parameters.particleRadius, 0.001f, 0.001f, 1.0f);

            ImGui::BeginDisabled();
            ImGui::DragFloat("Smoothing Radius", &parameters.smoothingRadius, 0.001f, 0.001f, 2.0f);
            ImGui::EndDisabled();

            ImGui::DragFloat("Rest Density", &parameters.restDensity, 0.1f, 0.0f, 100.0f);

            ImGui::DragFloat("Pressure Stiffness", &parameters.pressureStiffness, 0.0001f, 0.0f, 1.0f);

            ImGui::DragFloat( "Near Pressure Stiffness", &parameters.nearPressureStiffness, 0.0001f, 0.0f, 1.0f);

            ImGui::SeparatorText("Viscosity");

            ImGui::DragFloat("Sigma", &parameters.sigma, 0.01f, 0.0f, 10.0f);

            ImGui::DragFloat("Beta", &parameters.beta, 0.01f, 0.0f, 10.0f);

            ImGui::SeparatorText("Springs");

            ImGui::DragFloat("Gamma", &parameters.gamma, 0.01f, 0.0f, 10.0f);

            ImGui::DragFloat("Plasticity", &parameters.plasticity, 0.01f, 0.0f, 10.0f);

            ImGui::DragFloat("Spring Stiffness", &parameters.springStiffness, 0.01f, 0.0f, 10.0f);

            ImGui::SeparatorText("Stickiness");

            ImGui::DragFloat("Max Stickiness", &parameters.maxStickiness, 0.001f, 0.0f, 2.0f);

            ImGui::DragFloat("Stickiness Strength", &parameters.kStick, 0.01f, 0.0f, 10.0f);

            ImGui::SeparatorText("Gravity");

            ImGui::DragFloat("Gravity", &parameters.gravity, 0.001f, 0.0f, 10.0f);

            ImGui::DragFloat3("Gravity Direction", &parameters.gravityDirection.x, 0.01f, -1.0f, 1.0f);

            ImGui::SeparatorText("Simulation");

            ImGui::DragFloat("Fixed Delta Time", &parameters.fixedDeltaTime, 0.01f, 0.001f, 1.0f);

            ImGui::SeparatorText("Rendering");

            const std::string_view currentName = magic_enum::enum_name(m_particleColorMode);
            if (ImGui::BeginCombo("Particle Color", currentName.data()))
            {
                for (ParticleColorMode mode : magic_enum::enum_values<ParticleColorMode>())
                {
                    const std::string_view name = magic_enum::enum_name(mode);
                    const bool selected = mode == m_particleColorMode;
                    if (ImGui::Selectable(name.data(), selected))
                    {
                        m_particleColorMode = mode;
                    }
                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::DragFloat("Velocity Color Scale", &m_velocityColorScale, 0.01f, 0.001f, 10.0f);

            ImGui::DragFloat("Pressure Color Scale", &m_pressureColorScale, 0.001f, 0.001f, 10.0f);
        }

        ImGui::End();
    }
}
