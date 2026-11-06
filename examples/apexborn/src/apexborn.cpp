#include "apexborn.h"

#include "extensions/ImGuiRoadProfiler.h"
#include "layers/contexts/asset_context.h"
#include "layers/contexts/scene_context.h"
#include "models/mesh_factory.h"
#include "simulation/driving_observation.h"
#include "simulation/reference_driver.h"
#include "simulation/road_sensors.h"

#include <components/camera_component.h>
#include <components/mesh_component.h>
#include <intrumentor/profiler.h>

#include <imgui.h>
#include <types/draw_types.h>

#include <magic_enum/magic_enum.hpp>

#include <vector>
#include <random>

namespace ab
{

    ApexBorn::ApexBorn() = default;

    ApexBorn::~ApexBorn() = default;

    void ApexBorn::init()
    {

        scene().clearColor() = {0.10f, 0.16f, 0.13f};

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly, aiko::Camera::CameraType::Perspective);
        camera->getCamera().position = { 0.0f, 50.0f, 10.0f };
        camera->getCamera().target = {0.0f, 0.0f, 0.0f};
        camera->speed() *= 4.0f;

        scene().ambientLight().color = aiko::WHITE;
        scene().ambientLight().intensity = 0.3f;

        m_vehicleModel = assets().loadModel("porsche.fbx");

        m_track.init(assets());
        m_environment.init(assets());


        if (regenerateWorld() == false)
        {
            aiko::logger::Log::error("Couldn't generate ApexBorn world");
            m_trainingFailed = true;
        }

        if (m_trainingFailed == false)
        {
            if (resetTraining() == false)
            {
                aiko::logger::Log::error("Couldn't generate traingin session");
                m_trainingFailed = true;
            }
        }

    }

    void ApexBorn::update()
    {
        AIKO_FUNCTION_PROFILE

        if (m_trainingFailed)
        {
            return;
        }

        if (m_drivingMode == DrivingMode::Reference && m_episodeFinished)
        {
            return;
        }

        const float frameDt = std::clamp(getDeltaTime(), 0.0f, 0.1f);

        m_simulationAccumulator += frameDt;

        while (m_simulationAccumulator >= FixedTimeStep)
        {
            simulateStep(FixedTimeStep);
            if (m_trainingFailed == true)
            {
                return;
            }
            if (m_episodeFinished)
            {
                m_simulationAccumulator = 0.0f;
                break;
            }
            m_simulationAccumulator -= FixedTimeStep;
        }

        if ( m_drivingMode == DrivingMode::NEAT && m_episodeFinished == true)
        {
            if (m_trainingSession.finishGeneration(m_agents, m_track.getTrackLength(), MaxEpisodeDuration) == false)
            {
                m_trainingFailed = true;
                return;
            }
            if (m_trainingImgui.regenerateTrackEveryNGenerations > 0)
            {
                const auto generation = m_trainingSession.generation();
                if (generation % m_trainingImgui.regenerateTrackEveryNGenerations == 0)
                {
                    m_trainingImgui.nextTrackSeed++;
                    m_trackSettings.seed = m_trainingImgui.nextTrackSeed;
                    if (regenerateWorld() == false)
                    {
                        m_trainingFailed = true;
                    }
                    return;
                }
            }
            resetAgents();
        }

    }

    void ApexBorn::render()
    {
        AIKO_FUNCTION_PROFILE
        Layer::render();
        renderImgui();

        const aiko::LightData sun
        {
            .type = aiko::LightType::Directional,
            .direction = {-0.5f, -1.0f, -0.3f},
            .color = aiko::WHITE,
            .intensity = 1.0f,
        };
        renderer().submitLight(sun);

        m_environment.render(renderer());
        m_track.render(renderer());

        // Render vehicles

        // Render vehicles using CPU instancing
        constexpr float VehicleYawOffset = -90.0f;
        constexpr float VehicleScale = 0.0015f;

        if (m_vehicleModel.isValid() && !m_agents.empty())
        {
            std::vector<aiko::InstanceData> instances;
            instances.reserve(m_agents.size());

            for (const auto& agent : m_agents)
            {
                const auto& car = agent.car;

                instances.push_back(aiko::InstanceData
                {
                    .position = car.position(),
                    .rotation = { 0.0f, -aiko::math::degrees(car.heading()) + VehicleYawOffset, 0.0f },
                    .scale = { VehicleScale },
                    .color = aiko::WHITE,
                });
            }

            renderer().drawModelInstanced(m_vehicleModel, instances.data(), static_cast<aiko::u32>(instances.size()));

        }

        // Debug: visualize road sensors for the selected vehicle.
        if (m_vehicleImgui.renderSensors && m_selectedAgent >= 0 && static_cast<size_t>(m_selectedAgent) < m_agents.size())
        {
            const auto& agent = m_agents[static_cast<size_t>(m_selectedAgent)];

            const auto& vehicle = agent.car;
            const auto& readings = agent.sensors;

            constexpr aiko::vec3 DebugOffset = {0.0f, 0.12f, 0.0f};

            const auto origin = vehicle.position() + DebugOffset;

            for (const auto& reading : readings)
            {
                const auto end = reading.endPoint + DebugOffset;

                renderer().drawLine(origin, end, reading.hit ? aiko::GREEN : aiko::RED);

                if (reading.hit)
                {
                    renderer().drawPoint(end, aiko::YELLOW);
                }
            }
        }


    }

    bool ApexBorn::regenerateWorld()
    {
        if (!m_track.generate(m_trackSettings))
        {
            return false;
        }

        // Allow room beyond the procedural track generation area.
        constexpr float GroundPadding = 12.0f;

        const float margin = GroundPadding + m_trackSettings.maxDisplacement + m_trackSettings.trackWidth;

        const aiko::vec2 groundSize = {
            m_trackSettings.areaSize.x + margin * 2.0f,
            m_trackSettings.areaSize.y + margin * 2.0f
        };

        bool result =  m_environment.generate(m_track, groundSize, m_trackSettings.seed);

        if (result == false)
        {
            return false;
        }

        resetAgents();

        return true;

    }

    void ApexBorn::resetAgents()
    {

        m_episodeElapsedTime = 0.0f;
        m_episodeFinished = false;

        const auto finishLine = m_track.getFinishLine();

        const float heading = std::atan2(finishLine.forward.z, finishLine.forward.x);

        // Recreate the requested number of agents.
        m_agents.resize(static_cast<size_t>(m_trainingSession.populationSize()));

        std::mt19937 rng(m_trackSettings.seed);
        std::uniform_real_distribution<float> steeringBiasDistribution(-0.06f, 0.06f);
        std::uniform_real_distribution<float> throttleScaleDistribution(0.80f, 1.20f);

        for (size_t i = 0; i < m_agents.size(); ++i)
        {

            auto& agent = m_agents[i];

            // Every vehicle starts at the same position.
            agent.car.reset(finishLine.position, heading);

            agent.alive = true;
            agent.outcome = AgentOutcome::Running;

            agent.lastProgressCheckpoint = 0.0f;
            agent.timeWithoutProgress = 0.0f;
            agent.completionTime = 0.0f;

            const auto projection = m_track.projectPoint(agent.car.position());
            agent.previousProgress = projection.progressOnTrack;
            agent.sensors = RoadSensors::sense(agent.car, m_track);
            agent.observation = DrivingObservation::calculate(agent.car, m_track, projection, agent.sensors);

            agent.totalProgress = 0.0f;
            agent.bestProgress = 0.0f;

            // Agent 0 remains the unmodified reference driver.
            if (i == 0)
            {
                agent.referenceSteeringBias = 0.0f;
                agent.referenceThrottleScale = 1.0f;
            }
            else
            {
                agent.referenceSteeringBias = steeringBiasDistribution(rng);
                agent.referenceThrottleScale = throttleScaleDistribution(rng);
            }

        }

        // Reset simulation timing and UI selection.
        m_simulationAccumulator = 0.0f;
        m_selectedAgent = 0;
    }

    bool ApexBorn::resetTraining()
    {
        const EvolutionConfig config =
        {
            .populationSize = m_vehicleCount,
        };
        TrainingSession newSession;
        if (newSession.init(config, m_trackSettings.seed) == false)
        {
            aiko::logger::Log::error("Failed to regenerated new training session");
            return false;
        }
        m_trainingSession = std::move(newSession);
        resetAgents();

        m_trainingFailed = false;

        m_trainingImgui.nextTrackSeed = m_trackSettings.seed;

        return true;
    }

    void ApexBorn::simulateStep(float dt)
    {
        AIKO_FUNCTION_PROFILE

        m_episodeElapsedTime += dt;

        const float trackLength = m_track.getTrackLength();

        for (size_t i = 0 ; i < m_agents.size() ; ++i)
        {
            VehicleAgent& agent = m_agents[i];
            if (!agent.alive)
            {
                continue;
            }

            auto& car = agent.car;

            switch (m_drivingMode)
            {
                case DrivingMode::Reference:
                {
                    const auto control = ReferenceDriver::compute(car, m_track, agent.previousProgress);
                    const float steeringBias = m_referenceDriverVariation ? agent.referenceSteeringBias : 0.0f;
                    const float throttleScale = m_referenceDriverVariation ? agent.referenceThrottleScale : 1.0f;
                    car.setSteering(control.steering + steeringBias);
                    car.setThrottle(control.throttle * throttleScale);
                    car.setBrake(control.brake);
                }
                break;

                case DrivingMode::NEAT:
                {
                    VehicleControl control;
                    if (m_trainingSession.evaluate(i, agent.observation, control) == false)
                    {
                        aiko::logger::Log::error("Skipping driver as couldn't evaluate training session");
                        m_trainingFailed = true;
                        return;
                    }
                    car.setSteering(control.steering);
                    car.setThrottle(control.throttle);
                    car.setBrake(control.brake);
                }
                break;
                default:
                    AIKO_ASSERT(false, "Unknown/NotSupported driving mode");
            }

            car.update(dt);

            const auto projection = m_track.projectPoint(car.position());

            if (!m_track.isOnRoad(projection))
            {
                agent.alive = false;
                agent.outcome = AgentOutcome::OffRoad;
                continue;
            }

            agent.sensors = RoadSensors::sense(car, m_track);

            agent.observation = DrivingObservation::calculate(car, m_track, projection, agent.sensors);

            float delta = projection.progressOnTrack - agent.previousProgress;

            if (delta < -trackLength * 0.5f)
            {
                delta += trackLength;
            }
            else if (delta > trackLength * 0.5f)
            {
                delta -= trackLength;
            }

            agent.totalProgress += delta;
            agent.previousProgress = projection.progressOnTrack;
            agent.bestProgress = std::max(agent.bestProgress, agent.totalProgress);

            // Successfully completed one lap.
            if (agent.bestProgress >= trackLength)
            {
                agent.alive = false;
                agent.outcome = AgentOutcome::Completed;
                agent.completionTime = m_episodeElapsedTime;
                continue;
            }

            // Has the vehicle made meaningful new progress?
            if (agent.bestProgress - agent.lastProgressCheckpoint >= MinimumProgress)
            {
                agent.lastProgressCheckpoint = agent.bestProgress;
                agent.timeWithoutProgress = 0.0f;
            }
            else
            {
                agent.timeWithoutProgress += dt;
            }

            // Stop vehicles that are not progressing.
            if (agent.timeWithoutProgress >= StagnationTimeout)
            {
                agent.alive = false;
                agent.outcome = AgentOutcome::Stagnated;
                continue;
            }
        }

        // Global episode timeout.
        if (m_episodeElapsedTime >= MaxEpisodeDuration)
        {
            for (auto& agent : m_agents)
            {
                if (agent.alive)
                {
                    agent.alive = false;
                    agent.outcome = AgentOutcome::TimedOut;
                }
            }
        }

        // Episode finishes when no active agents remain.
        m_episodeFinished = std::all_of(
            m_agents.begin(),
            m_agents.end(),
            [](const VehicleAgent& agent)
            {
                return !agent.alive;
            }
        );

    }

    void ApexBorn::renderImgui()
    {
        renderMenuBar();
        if (m_trackImgui.isOpen)
        {
            renderTrackGenerationSettings();
        }
        if (m_vehicleImgui.isOpen)
        {
            renderVehicleDebug();
        }
        if (m_trainingImgui.isOpen)
        {
            renderTrainingSimulation();
        }
        m_profileEditor.render(m_track.roadProfile());
    }

    void ApexBorn::renderMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Tools"))
            {
                ImGui::MenuItem("RoadGenerationSettings", nullptr, &m_trackImgui.isOpen);
                ImGui::MenuItem("VehicleEditorSettings", nullptr, &m_vehicleImgui.isOpen);
                ImGui::MenuItem("TrainingSessionSettings", nullptr, &m_trainingImgui.isOpen);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void ApexBorn::renderTrackGenerationSettings()
    {
        if (ImGui::Begin("Road Generation Settings", &m_trackImgui.isOpen) == false)
        {
            ImGui::End();
            return;
        }

        ImGui::SeparatorText("Road Profile");

        if (ImGui::Button("Open Profile Editor"))
        {
            if (m_profileEditor.isOpen() == false)
            {
                m_profileEditor.open(m_track.roadProfile());
            }
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Track Geometry");

        constexpr std::uint32_t MinPoints = 4;
        constexpr std::uint32_t MaxPoints = 100;

        ImGui::DragFloat("Track Width", &m_trackSettings.trackWidth, 0.025f, 0.1f, 10.0f);
        ImGui::DragFloat2("Area Size", &m_trackSettings.areaSize.x, 0.25f, 1.0f, 500.0f);
        ImGui::DragFloat("Min Distance", &m_trackSettings.minDistance, 0.05f, 0.0f, 20.0f);
        ImGui::DragFloat("Max Displacement", &m_trackSettings.maxDisplacement, 0.05f, 0.0f, 50.0f);
        ImGui::DragFloat("Min Control Point Distance", &m_trackSettings.minControlPointDistance, 0.05f, 0.0f, 20.0f);
        ImGui::SliderFloat("Max Turn Angle", &m_trackSettings.maxTurnAngle, 1.0f, 180.0f, "%.1f deg");

        ImGui::Spacing();
        ImGui::SeparatorText("Procedural Generation");

        ImGui::InputScalar("Track Seed", ImGuiDataType_U32, &m_trackSettings.seed);
        ImGui::DragScalar("Random Points", ImGuiDataType_U32, &m_trackSettings.controlPointCount, 1.0f, &MinPoints, &MaxPoints, "%u");
        ImGui::Spacing();
        ImGui::SeparatorText("Debug");

        ImGui::Checkbox("Enable Debug Render", &m_track.isDebugEnabled());

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Regenerate Track"))
        {
            if (regenerateWorld() == false)
            {
                aiko::logger::Log::error("Couldn't regenerate ApexBorn world");
            }
        }

        ImGui::End();
    }

    void ApexBorn::renderVehicleDebug()
    {
        if (ImGui::Begin("Vehicle Debug", &m_vehicleImgui.isOpen) == false)
        {
            ImGui::End();
            return;
        }

        ImGui::SeparatorText("Visualization");

        ImGui::Checkbox("Render Road Sensors", &m_vehicleImgui.renderSensors);
        ImGui::SeparatorText("Vehicle Selection");

        ImGui::Text("Total Vehicles: %zu", m_agents.size());

        if (m_agents.empty())
        {
            ImGui::TextDisabled("No vehicles available.");
            ImGui::End();
            return;
        }

        ImGui::SliderInt("Selected Vehicle", &m_selectedAgent, 0, static_cast<int>(m_agents.size()) - 1);

        m_selectedAgent = std::clamp(m_selectedAgent, 0, static_cast<int>(m_agents.size()) - 1);

        const auto& agent = m_agents[static_cast<std::size_t>(m_selectedAgent)];
        const auto& car = agent.car;
        const auto& observation = agent.observation;

        const auto projection = m_track.projectPoint(car.position());
        const float trackLength = m_track.getTrackLength();

        ImGui::Spacing();
        ImGui::SeparatorText("Vehicle State");

        ImGui::Text("Speed: %.2f m/s", car.speed());
        ImGui::Text("Steering: %.3f", car.steering());
        ImGui::Text("Throttle: %.3f", car.throttle());
        ImGui::Text("Brake: %.3f", car.brake());

        ImGui::Text("Lateral Offset: %.2f", projection.signedLateralOffset);
        ImGui::Text("Track Progress: %.2f", projection.progressOnTrack);
        ImGui::Text("Total Progress: %.2f", agent.totalProgress);

        const float alignment = aiko::math::dot(car.forwardDirection(), projection.forwardDirection);

        ImGui::Text("Heading Alignment: %.3f", alignment);

        ImGui::Spacing();
        ImGui::SeparatorText("Episode Performance");

        const float score = trackLength > 0.0f ? std::clamp(agent.bestProgress / trackLength, 0.0f, 1.0f) : 0.0f;
        const std::size_t completedLaps = trackLength > 0.0f ? static_cast<std::size_t>(std::floor(agent.bestProgress / trackLength)) : 0;

        ImGui::Text("Best Progress: %.2f m", agent.bestProgress);
        ImGui::Text("Completed Laps: %zu", completedLaps);
        ImGui::Text("Stagnation Time: %.2f s", agent.timeWithoutProgress);
        ImGui::Text("Driving Score: %.3f", score);
        ImGui::ProgressBar(score, ImVec2(-FLT_MIN, 0.0f));

        const std::string_view outcome = magic_enum::enum_name(agent.outcome);

        const bool success = agent.outcome == AgentOutcome::Completed;
        const bool running = agent.outcome == AgentOutcome::Running;
        const ImVec4 outcomeColor = success || running ? ImVec4(0.2f, 0.9f, 0.3f, 1.0f) : ImVec4(0.9f, 0.2f, 0.2f, 1.0f);

        ImGui::TextColored(outcomeColor, "Outcome: %s", outcome.data());

        if (success)
        {
            ImGui::Text("Completion Time: %.2f s", agent.completionTime);
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Driving Observations");

        const auto showObservation = [](const char* label, float value, float min, float max)
        {
            const float normalized = aiko::math::clamp((value - min) / (max - min), 0.0f, 1.0f);
            ImGui::Text("%s: %.3f", label, value);
            ImGui::ProgressBar(normalized, ImVec2(-FLT_MIN, 0.0f), "");
        };

        showObservation("Lateral Offset", observation.lateralOffset, -1.0f, 1.0f);
        showObservation("Heading Error", observation.headingError, -1.0f, 1.0f);
        showObservation("Near LookAhead", observation.lookAheadError, -1.0f, 1.0f);
        showObservation("Far LookAhead", observation.farLookAheadError, -1.0f, 1.0f);
        showObservation("Upcoming Curvature", observation.upcomingCurvature, 0.0f, 1.0f);
        showObservation("Normalized Speed", observation.normalizedSpeed, 0.0f, 1.0f);

        ImGui::Spacing();
        ImGui::SeparatorText("Road Sensors");

        constexpr const char* SensorNames[] = {
            "-90 degrees",
            "-45 degrees",
            "Forward",
            "+45 degrees",
            "+90 degrees"
        };

        for (std::size_t i = 0; i < RoadSensors::SensorCount; ++i)
        {
            ImGui::Text("%s: %.3f (%s)", SensorNames[i], observation.roadDistances[i], agent.sensors[i].hit ? "HIT" : "MISS");
        }

        ImGui::End();
    }

    void ApexBorn::renderTrainingSimulation()
    {
        if (ImGui::Begin("Training Session", &m_trainingImgui.isOpen) == false)
        {
            ImGui::End();
            return;
        }

        ImGui::SeparatorText("Driving Mode");

        {
            const std::string_view currentName = magic_enum::enum_name(m_drivingMode);
            if (ImGui::BeginCombo("Controller", currentName.data()))
            {
                for (const DrivingMode mode : magic_enum::enum_values<DrivingMode>())
                {
                    const std::string_view name = magic_enum::enum_name(mode);
                    const bool selected = mode == m_drivingMode;
                    if (ImGui::Selectable(name.data(), selected) && !selected)
                    {
                        m_drivingMode = mode;
                        resetAgents();
                    }
                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }

        ImGui::TextDisabled("%s", m_drivingMode == DrivingMode::NEAT ? "Vehicles controlled by evolved neural networks." : "Vehicles controlled by the reference driver.");

        ImGui::Spacing();
        ImGui::SeparatorText("Simulation");

        int interval = static_cast<int>(m_trainingImgui.regenerateTrackEveryNGenerations);
        if (ImGui::SliderInt("Regenerate Every N Generations", &interval, 0, 100))
        {
            m_trainingImgui.regenerateTrackEveryNGenerations = static_cast<std::size_t>(interval);
        }
        ImGui::TextDisabled("0 disables automatic track regeneration.");

        ImGui::Text("Active Vehicles: %zu", m_agents.size());
        ImGui::Text("Episode Time: %.2f / %.2f s", m_episodeElapsedTime, MaxEpisodeDuration);

        if (ImGui::Button("Reset Episode"))
        {
            resetAgents();
        }

        if (m_drivingMode == DrivingMode::Reference)
        {
            ImGui::Spacing();
            ImGui::SeparatorText("Reference Driver");
            ImGui::Checkbox("Driver Variation", &m_referenceDriverVariation);
            ImGui::TextDisabled("Reference mode does not evolve the NEAT population.");
            ImGui::End();
            return;
        }

        ImGui::Spacing();
        ImGui::SeparatorText("NEAT Population");

        ImGui::Text("Generation: %llu", static_cast<unsigned long long>(m_trainingSession.generation()));
        ImGui::Text("Active Population: %zu", m_trainingSession.populationSize());
        ImGui::SliderInt("Population Size", &m_vehicleCount, 1, 150);

        if (static_cast<std::size_t>(m_vehicleCount) != m_trainingSession.populationSize())
        {
            ImGui::TextDisabled("Population size changes apply on the next training restart.");
        }

        ImGui::Spacing();

        if (ImGui::Button("Restart Training"))
        {
            if (resetTraining() == false)
            {
                aiko::logger::Log::error("Failed to restart NEAT training");
            }
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Training Status");

        if (m_trainingFailed)
        {
            ImGui::TextColored(ImVec4(0.95f, 0.3f, 0.3f, 1.0f), "Failed");
            ImGui::TextDisabled("Training has stopped. Restart training to try again.");
        }
        else if (m_episodeFinished)
        {
            ImGui::TextColored(ImVec4(0.95f, 0.75f, 0.25f, 1.0f), "Episode Complete");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.3f, 0.85f, 0.4f, 1.0f), "Running");
        }

        ImGui::End();
    }

}

