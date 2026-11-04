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
        }

    }

    void ApexBorn::update()
    {
        AIKO_FUNCTION_PROFILE
        const float frameDt = std::clamp(getDeltaTime(), 0.0f, 0.1f);
        m_simulationAccumulator += frameDt;
        while (m_simulationAccumulator >= FixedTimeStep)
        {
            simulateStep(FixedTimeStep);
            m_simulationAccumulator -= FixedTimeStep;
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
        if (m_trackImgui.openVehicleEditor && m_selectedAgent >= 0 && static_cast<size_t>(m_selectedAgent) < m_agents.size())
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
        const auto finishLine = m_track.getFinishLine();

        const float heading = std::atan2(finishLine.forward.z, finishLine.forward.x);

        // Recreate the requested number of agents.
        m_agents.resize(static_cast<size_t>(m_vehicleCount));

        std::mt19937 rng(m_trackSettings.seed);
        std::uniform_real_distribution<float> steeringBiasDistribution(-0.06f, 0.06f);
        std::uniform_real_distribution<float> throttleScaleDistribution(0.80f, 1.20f);

        for (size_t i = 0; i < m_agents.size(); ++i)
        {

            auto& agent = m_agents[i];

            // Every vehicle starts at the same position.
            agent.car.reset(finishLine.position, heading);

            agent.alive = true;

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


    void ApexBorn::simulateStep(float dt)
    {
        AIKO_FUNCTION_PROFILE
        const float trackLength = m_track.getTrackLength();

        for (auto& agent : m_agents)
        {
            if (!agent.alive)
            {
                continue;
            }

            auto& car = agent.car;

            if (m_autonomousDriving)
            {
                const auto control = ReferenceDriver::compute(car, m_track, agent.previousProgress);
                const float steeringBias = m_referenceDriverVariation ? agent.referenceSteeringBias : 0.0f;
                const float throttleScale = m_referenceDriverVariation ? agent.referenceThrottleScale : 1.0f;
                car.setSteering(control.steering + steeringBias);
                car.setThrottle(control.throttle * throttleScale);
                car.setBrake(control.brake);
            }

            car.update(dt);

            const auto projection = m_track.projectPoint(car.position());

            if (!m_track.isOnRoad(projection))
            {
                agent.alive = false;
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
            agent.bestProgress = std::max( agent.bestProgress, agent.totalProgress);
        }
    }

    void ApexBorn::renderImgui()
    {
        renderMenuBar();
        if (m_trackImgui.isOpen)
        {
            renderTrackGenerationSettings();
        }
        if (m_trackImgui.openVehicleEditor)
        {
            renderVehicleDebug();
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
                ImGui::MenuItem("VehicleEditorSettings", nullptr, &m_trackImgui.openVehicleEditor);
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

        if (ImGui::Button("Open Profile Editor"))
        {
            if (!m_profileEditor.isOpen())
            {
                m_profileEditor.open(m_track.roadProfile());
            }
        }

        const uint32_t minPoints = 4;
        const uint32_t maxPoints = 100;

        ImGui::DragFloat("Track Width", &m_trackSettings.trackWidth, 0.025f, 0.1f, 10.0f);

        ImGui::InputScalar("Track Seed", ImGuiDataType_U32, &m_trackSettings.seed);
        ImGui::DragScalar("Random Points", ImGuiDataType_U32, &m_trackSettings.controlPointCount, 1.0f, &minPoints, &maxPoints, "%u");

        ImGui::DragFloat2("Area Size", &m_trackSettings.areaSize.x, 0.25f, 1.0f, 500.0f);
        ImGui::DragFloat("Min Distance", &m_trackSettings.minDistance, 0.05f, 0.0f, 20.0f);
        ImGui::DragFloat("Max Displacement", &m_trackSettings.maxDisplacement, 0.05f, 0.0f, 50.0f);
        ImGui::DragFloat("Min Control Point Distance", &m_trackSettings.minControlPointDistance, 0.05f, 0.0f, 20.0f);
        ImGui::SliderFloat("Max Turn Angle", &m_trackSettings.maxTurnAngle, 1.0f, 180.0f, "%.1f deg");

        ImGui::Checkbox("Enable Debug Render", &m_track.isDebugEnabled());

        if (ImGui::Button("Regenerate Track"))
        {
            if (!regenerateWorld())
            {
                aiko::logger::Log::error("Couldn't regenerate ApexBorn world");
            }
        }

        ImGui::End();
    }

    void ApexBorn::renderVehicleDebug()
    {
        if (ImGui::Begin("Vehicle Debug") == false)
        {
            ImGui::End();
            return;
        }

        if (ImGui::Checkbox("Autonomous Driving", &m_autonomousDriving))
        {
            if (!m_autonomousDriving)
            {
                for (auto& agent : m_agents)
                {
                    agent.car.setSteering(0.0f);
                    agent.car.setThrottle(0.0f);
                    agent.car.setBrake(0.0f);
                }
            }
        }

        ImGui::Checkbox("Reference Driver Variation", &m_referenceDriverVariation);

        // Vehicle count is applied on the next reset.
        ImGui::SliderInt("Vehicle Count (next reset)", &m_vehicleCount, 1, 100);

        if (ImGui::Button("Reset Episode"))
        {
            resetAgents();
        }

        ImGui::SeparatorText("Vehicle Selection");

        ImGui::Text("Total Vehicles: %zu", m_agents.size());

        if (m_agents.empty())
        {
            ImGui::TextDisabled("No vehicles available.");
            ImGui::End();
            return;
        }

        ImGui::SliderInt("Selected Vehicle", &m_selectedAgent, 0, static_cast<int>(m_agents.size()) - 1);

        auto& agent = m_agents[static_cast<size_t>(m_selectedAgent)];
        auto& car = agent.car;

        const auto projection = m_track.projectPoint(car.position());
        ImGui::Text("Lateral Offset: %.2f", projection.signedLateralOffset);
        ImGui::Text("Track Progress: %.2f", projection.progressOnTrack);
        ImGui::Text("Total Progress: %.2f", agent.totalProgress);
        const float trackLength = m_track.getTrackLength();
        const size_t completedLaps = trackLength > 0.0f ? static_cast<size_t>(std::floor(agent.bestProgress / trackLength)) : 0;
        ImGui::Text("Best Progress: %.2f", agent.bestProgress);
        ImGui::Text("Completed Laps: %zu", completedLaps);
        ImGui::Text("Speed: %f m/s", car.speed());
        const float alignment = aiko::math::dot(car.forwardDirection(), projection.forwardDirection);
        ImGui::Text("Heading Alignment: %.3f", alignment);
        ImGui::TextColored(agent.alive ? ImVec4(0.2f, 0.9f, 0.3f, 1.0f) : ImVec4(0.9f, 0.2f, 0.2f, 1.0f), "%s", agent.alive ? "ACTIVE" : "FAILED");

        ImGui::BeginDisabled(!agent.alive || m_autonomousDriving);
        float currentSteering = car.steering();
        if (ImGui::SliderFloat("Steering", &currentSteering, -1.0f, 1.0f))
        {
            car.setSteering(currentSteering);
        }
        float throttle = car.throttle();
        if (ImGui::SliderFloat("Throttle", &throttle, 0.0f, 1.0f, "%.3f"))
        {
            car.setThrottle(throttle);
        }
        float brake = car.brake();
        if (ImGui::SliderFloat("Brake", &brake, 0.0f, 1.0f, "%.3f"))
        {
            car.setBrake(brake);
        }
        ImGui::EndDisabled();

        // Driving observations
        const auto& observation = agent.observation;
        ImGui::SeparatorText("Driving Observations");

        const auto showObservation = [](const char* label, float value, float min, float max)
        {
            const float normalized = aiko::math::clamp( (value - min) / (max - min), 0.0f, 1.0f);
            ImGui::Text("%s: %.3f", label, value);
            ImGui::ProgressBar(normalized, ImVec2(-FLT_MIN, 0.0f), "");
        };

        showObservation("Lateral Offset", observation.lateralOffset, -1.0f, 1.0f);
        showObservation("Heading Error", observation.headingError, -1.0f, 1.0f);
        showObservation("Near LookAhead", observation.lookAheadError, -1.0f, 1.0f);
        showObservation("Far LookAhead", observation.farLookAheadError, -1.0f, 1.0f);
        showObservation("Upcoming Curvature", observation.upcomingCurvature, 0.0f, 1.0f);
        showObservation("Normalized Speed", observation.normalizedSpeed, 0.0f, 1.0f);

        ImGui::SeparatorText("Road Sensors");

        constexpr const char* SensorNames[] = {
            "-90 degrees",
            "-45 degrees",
            "Forward",
            "+45 degrees",
            "+90 degrees"
        };

        for (size_t i = 0; i < RoadSensors::SensorCount; ++i)
        {
            ImGui::Text("%s: %.3f (%s)", SensorNames[i], observation.roadDistances[i], agent.sensors[i].hit ? "HIT" : "MISS");
        }

        ImGui::End();
    }

}

