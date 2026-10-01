#include "playground.h"

#include "assets/types/mesh_asset.h"
#include "intrumentor/profiler.h"
#include "layers/contexts/render_context.h"
#include "models/mesh_factory.h"
#include "systems/render_system.h"
#include "systems/asset_system.h"
#include "time/time.h"

#include <magic_enum/magic_enum.hpp>
#include <imgui.h>

#include <algorithm>

namespace sph
{

    namespace
    {
        constexpr bool EnableGpuSimulation = true;
        constexpr uint32_t MaxSimulationSubsteps = 4;
    }

    Playground::Playground() = default;
    Playground::~Playground() = default;

    void Playground::addShapeRenderData(const Shape& shape)
    {
        ShapeRenderData& renderData = m_shapeRenderData.emplace_back();
        renderData.mesh.upload(shape.asset());
        renderData.material.m_shaderId = m_shapeShaderId;
        renderData.material.m_useVertexColor = false;
        renderData.material.m_lit = false;
        renderData.material.m_baseColor = shape.color();
    }

    void Playground::rebuildShapeRenderData()
    {
        m_shapeRenderData.clear();

        for (const Shape& shape : m_simulation.shapes())
        {
            addShapeRenderData(shape);
        }
    }

    void Playground::init(const aiko::AssetId& shaderId, const aiko::AssetId& gpuShaderId, aiko::AssetSystem& assetSystem)
    {
        m_simulation.init();
        m_gpuSimulation.init(assetSystem, m_simulation.particles(), m_simulation.shapes());

        m_shapeShaderId = shaderId;

        const auto& shapes = m_simulation.shapes();

        m_shapeRenderData.clear();

        for (const Shape& shape : shapes)
        {
            addShapeRenderData(shape);
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

        const ImGuiIO& io = ImGui::GetIO();

        if (io.WantCaptureMouse || io.WantCaptureKeyboard)
        {
            m_previousMousePosition = mousePosition;
            return;
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
            SPHParameters& parameters = m_simulation.parameters();

            const float simulationStepTime = parameters.simulationStepTime;

            AIKO_ASSERT(simulationStepTime > 0.0f, "SPH simulation step time must be greater than zero");
            AIKO_ASSERT(parameters.solverDeltaTime > 0.0f, "SPH solver delta time must be greater than zero");

            const float frameDeltaTime = aiko::Time::it().getDeltaTime();

            m_simulationAccumulator += frameDeltaTime;

            m_lastSimulationSubsteps = 0;

            m_gpuSimulation.updateShapes(m_simulation.shapes());

            aiko::vector<SPHParticle> spawnedParticles;

            while (m_simulationAccumulator >= simulationStepTime &&m_lastSimulationSubsteps < MaxSimulationSubsteps)
            {
                spawnedParticles.clear();

                m_simulation.updateEmitters(simulationStepTime, spawnedParticles);

                m_gpuSimulation.spawnParticles(spawnedParticles);

                m_gpuSimulation.update( renderSystem, parameters, m_simulation.bounds());
                m_simulationAccumulator -= simulationStepTime;
                ++m_lastSimulationSubsteps;
            }

            if (m_lastSimulationSubsteps == MaxSimulationSubsteps && m_simulationAccumulator >= simulationStepTime)
            {
                m_simulationAccumulator = 0.0f;
            }

            aiko::GpuInstanceDrawDesc draw{};
            draw.mesh = &m_particleMesh;
            draw.material = &m_gpuParticleMaterial;
            draw.readBuffers.push_back({7, &m_gpuSimulation.positionBuffer()});
            draw.readBuffers.push_back({8, &m_gpuSimulation.velocityBuffer()});
            draw.readBuffers.push_back({9, &m_gpuSimulation.pressureBuffer()});
            draw.instanceCount = m_gpuSimulation.particleCount();

            m_gpuParticleMaterial.setUInt("u_particleColorMode", static_cast<uint32_t>(m_particleColorMode));
            m_gpuParticleMaterial.setFloat("u_velocityColorScale", m_velocityColorScale);
            m_gpuParticleMaterial.setFloat("u_pressureColorScale", m_pressureColorScale);
            m_gpuParticleMaterial.setVec4("u_particleColor", m_particleColor.toVec4());
            m_gpuParticleMaterial.setVec4("u_velocityStartColor", m_velocityStartColor.toVec4());
            m_gpuParticleMaterial.setVec4("u_velocityEndColor", m_velocityEndColor.toVec4());
            m_gpuParticleMaterial.setVec4("u_pressureStartColor", m_pressureStartColor.toVec4());
            m_gpuParticleMaterial.setVec4("u_pressureEndColor", m_pressureEndColor.toVec4());

            renderer.drawMeshInstancedGpu(draw);
        }

        // Emitters
        const auto& emitters =
            m_simulation.emitters();

        for (size_t i = 0; i < emitters.size(); ++i)
        {
            const ParticleEmitter& emitter = emitters[i];

            const EmitterSettings& settings = emitter.settings();

            const aiko::vec3 direction = settings.direction;

            const float directionLengthSquared = aiko::math::dot(direction, direction);

            if (directionLengthSquared <= 1e-6f)
            {
                continue;
            }

            const aiko::vec3 normalizedDirection = aiko::math::normalize(direction);

            const aiko::vec3 normal = { -normalizedDirection.y, normalizedDirection.x, 0.0f};

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
            transform.scale = shape.scale();

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

            if (ImGui::Button("Reset Parameters"))
            {
                parameters = SPHParameters{};
            }

            ImGui::SameLine();
            ImGui::TextDisabled("Tuned defaults");

            ImGui::SeparatorText("Fluid");

            ImGui::BeginDisabled();

            ImGui::SliderFloat("Particle Radius", &parameters.particleRadius, 0.01f, 0.20f, "%.3f");

            ImGui::SliderFloat("Smoothing Radius", &parameters.smoothingRadius, 0.05f, 1.0f, "%.3f");

            ImGui::EndDisabled();

            ImGui::TextDisabled("Particle and smoothing radii are locked at runtime.");

            ImGui::SliderFloat("Rest Density", &parameters.restDensity, 1.0f, 30.0f, "%.2f");

            ImGui::SliderFloat("Pressure Stiffness", &parameters.pressureStiffness, 0.0f, 0.02f, "%.5f");

            ImGui::SliderFloat("Near Pressure Stiffness", &parameters.nearPressureStiffness, 0.0f, 0.10f, "%.4f");

            ImGui::SeparatorText("Viscosity");

            ImGui::SliderFloat("Sigma", &parameters.sigma, 0.0f, 1.0f, "%.3f");

            ImGui::SliderFloat("Beta", &parameters.beta, 0.0f, 0.5f, "%.3f");

            ImGui::SliderFloat("Velocity Damping", &parameters.velocityDamping, 0.98f, 1.0f, "%.4f");

            ImGui::SliderFloat("Max Step Distance", &parameters.maxStepDisplacementRatio, 0.25f, 1.0f, "%.2f x smoothing radius");

            ImGui::SeparatorText("Springs");

            ImGui::SliderFloat("Gamma", &parameters.gamma, 0.0f, 1.0f, "%.3f");

            ImGui::SliderFloat("Plasticity", &parameters.plasticity, 0.0f, 2.0f, "%.3f");

            ImGui::SliderFloat("Spring Stiffness", &parameters.springStiffness, 0.0f, 1.0f, "%.3f");

            ImGui::SeparatorText("Stickiness");

            ImGui::SliderFloat("Max Stickiness", &parameters.maxStickiness, 0.0f, parameters.smoothingRadius, "%.3f");

            ImGui::SliderFloat("Stickiness Strength", &parameters.kStick, 0.0f, 0.5f, "%.3f");

            ImGui::SeparatorText("Gravity");

            ImGui::SliderFloat("Gravity", &parameters.gravity, 0.0f, 0.05f, "%.4f");

            ImGui::SliderFloat3("Gravity Direction", &parameters.gravityDirection.x, -1.0f, 1.0f, "%.2f");

            ImGui::SeparatorText("Simulation");

            ImGui::Text("Simulation Step Time: %.6f s", parameters.simulationStepTime);

            ImGui::Text("Simulation Hz: %.1f", parameters.simulationStepTime > 0.0f ? 1.0f / parameters.simulationStepTime : 0.0f);

            ImGui::Text("Solver Delta Time: %.3f", parameters.solverDeltaTime);

            ImGui::TextDisabled("Simulation timing is locked at runtime.");

            ImGui::Text("Substeps this frame: %u / %u", m_lastSimulationSubsteps, MaxSimulationSubsteps);

            ImGui::Text("Accumulator: %.3f ms", m_simulationAccumulator * 1000.0f);

            ImGui::Text("Frame delta: %.3f ms", aiko::Time::it().getDeltaTime() * 1000.0f);

            ImGui::SeparatorText("Emitters");

            ImGui::Text("Emitters: %zu", m_simulation.emitters().size());

            ImGui::TextDisabled("At GPU capacity, new particles recycle the oldest particle slots.");

            if (ImGui::Button("Add Emitter"))
            {
                const EmitterSettings settings
                {
                    .position ={ 0.0f, 5.0f, 0.0f},
                    .direction ={ 0.0f, -1.0f, 0.0f},
                    .size = 2.0f,
                    .spawnInterval = 0.25f,
                    .amount = 10,
                    .velocity = 1.0f,
                    .angularVelocity = 0.0f,
                };

                m_simulation.createParticleEmitter(settings);

                m_selectedEmitter = m_simulation.emitters().size() - 1;
            }

            if (m_simulation.emitters().empty() == false)
            {
                ImGui::BeginChild("EmitterList", ImVec2(0.0f, 110.0f), ImGuiChildFlags_Borders);

                for (size_t i = 0; i < m_simulation.emitters().size(); ++i)
                {
                    const bool selected = m_selectedEmitter.has_value() && m_selectedEmitter.value() == i;

                    const std::string label = "Emitter " + std::to_string(i);

                    if (ImGui::Selectable(label.c_str(), selected))
                    {
                        m_selectedEmitter = i;
                    }
                }

                ImGui::EndChild();
            }

            if (m_selectedEmitter.has_value() && m_selectedEmitter.value() < m_simulation.emitters().size())
            {
                const size_t emitterIndex = m_selectedEmitter.value();

                ParticleEmitter& emitter = m_simulation.emitters()[emitterIndex];

                EmitterSettings& settings = emitter.settings();

                ImGui::SeparatorText("Selected Emitter");

                ImGui::DragFloat2("Position", &settings.position.x, 0.05f, -16.0f, 16.0f, "%.2f");

                if (ImGui::SliderFloat2("Direction", &settings.direction.x, -1.0f, 1.0f, "%.2f"))
                {
                    const float directionLengthSquared = aiko::math::dot(settings.direction, settings.direction);

                    if (directionLengthSquared <= 1e-6f)
                    {
                        settings.direction ={ 0.0f, -1.0f, 0.0f};
                    }
                    else
                    {
                        settings.direction = aiko::math::normalize(settings.direction);
                    }
                }

                ImGui::SliderFloat("Emitter Size", &settings.size, 0.1f, 10.0f, "%.2f");

                ImGui::SliderFloat( "Spawn Interval", &settings.spawnInterval, 0.01f, 5.0f, "%.2f s");

                int amount = static_cast<int>(settings.amount);

                if (ImGui::SliderInt("Particles Per Burst", &amount, 1, 128))
                {
                    settings.amount = static_cast<size_t>(amount);
                }

                ImGui::SliderFloat("Emitter Velocity", &settings.velocity, 0.0f, 10.0f, "%.2f");

                ImGui::SliderFloat("Angular Velocity", &settings.angularVelocity, -5.0f, 5.0f, "%.2f");

                if (ImGui::Button("Delete Emitter"))
                {
                    m_simulation.removeParticleEmitter(emitterIndex);

                    m_selectedEmitter.reset();
                }
            }

            ImGui::SeparatorText("Shapes");

            ImGui::Text("Shapes: %zu", m_simulation.shapes().size());

            if (ImGui::Button("Add Circle"))
            {
                m_simulation.createShape({0.0f, 0.0f, 0.0f}, aiko::mesh::factory::generateCircle(24), aiko::MAGENTA);

                addShapeRenderData(m_simulation.shapes().back());

                m_selectedShape = m_simulation.shapes().size() - 1;
            }

            ImGui::SameLine();

            if (ImGui::Button("Add Triangle"))
            {
                m_simulation.createShape({0.0f, 0.0f, 0.0f}, aiko::mesh::factory::generateTriangle(), aiko::MAGENTA);

                addShapeRenderData(m_simulation.shapes().back());

                m_selectedShape = m_simulation.shapes().size() - 1;
            }

            ImGui::SameLine();

            if (ImGui::Button("Add Rectangle"))
            {
                m_simulation.createShape( {0.0f, 0.0f, 0.0f}, aiko::mesh::factory::generateQuad(), aiko::MAGENTA);

                addShapeRenderData(m_simulation.shapes().back());

                m_selectedShape = m_simulation.shapes().size() - 1;
            }

            if (m_simulation.shapes().empty() == false)
            {
                ImGui::BeginChild( "ShapeList", ImVec2(0.0f, 110.0f), ImGuiChildFlags_Borders);

                for (size_t i = 0; i < m_simulation.shapes().size(); ++i)
                {
                    const bool selected = m_selectedShape.has_value() && m_selectedShape.value() == i;

                    const std::string label = "Shape " + std::to_string(i);

                    if (ImGui::Selectable(label.c_str(), selected))
                    {
                        m_selectedShape = i;
                    }
                }

                ImGui::EndChild();
            }

            if (m_selectedShape.has_value() && m_selectedShape.value() < m_simulation.shapes().size())
            {
                const size_t shapeIndex = m_selectedShape.value();

                Shape& shape = m_simulation.shapes()[shapeIndex];

                ImGui::SeparatorText("Selected Shape");

                aiko::vec3 shapePosition = shape.position();

                if (ImGui::DragFloat2("Shape Position", &shapePosition.x, 0.05f, -16.0f, 16.0f, "%.2f"))
                {
                    shape.setPosition(shapePosition);
                }

                aiko::vec3 shapeScale = shape.scale();

                if (ImGui::DragFloat2("Shape Size", &shapeScale.x, 0.05f, 0.10f, 10.0f, "%.2f"))
                {
                    shapeScale.x = std::max(shapeScale.x, 0.10f);
                    shapeScale.y = std::max(shapeScale.y, 0.10f);
                    shapeScale.z = 1.0f;

                    shape.setScale(shapeScale);
                }

                aiko::Color shapeColor = shape.color();

                if (ImGui::ColorEdit4("Shape Color", &shapeColor.r))
                {
                    shape.setColor(shapeColor);

                    m_shapeRenderData[shapeIndex].material.m_baseColor = shapeColor;
                }

                if (ImGui::Button("Delete Shape"))
                {
                    m_simulation.removeShape(shapeIndex);

                    rebuildShapeRenderData();

                    m_selectedShape.reset();
                }
            }

            ImGui::SeparatorText("Rendering");

            const std::string_view currentName = magic_enum::enum_name(m_particleColorMode);

            if (ImGui::BeginCombo("Particle Color Mode", currentName.data()))
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

            switch (m_particleColorMode)
            {
                case ParticleColorMode::Constant:
                {
                    ImGui::ColorEdit4("Particle Color", &m_particleColor.r);
                    break;
                }

                case ParticleColorMode::Velocity:
                {
                    ImGui::ColorEdit4("Velocity Start Color", &m_velocityStartColor.r);
                    ImGui::ColorEdit4("Velocity End Color", &m_velocityEndColor.r);
                    ImGui::DragFloat("Velocity Color Scale", &m_velocityColorScale, 0.01f, 0.001f, 10.0f, "%.3f");
                    break;
                }

                case ParticleColorMode::Pressure:
                {
                    ImGui::ColorEdit4("Pressure Start Color", &m_pressureStartColor.r);
                    ImGui::ColorEdit4("Pressure End Color", &m_pressureEndColor.r);
                    ImGui::DragFloat("Pressure Color Scale", &m_pressureColorScale, 0.001f, 0.001f, 10.0f, "%.3f");
                    break;
                }
            }
        }

        ImGui::End();
    }
}
