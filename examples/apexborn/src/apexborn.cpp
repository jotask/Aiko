#include "apexborn.h"

#include "extensions/ImGuiRoadProfiler.h"
#include "layers/contexts/asset_context.h"
#include "layers/contexts/scene_context.h"
#include "models/mesh_factory.h"
#include "simulation/driving_observation.h"
#include "simulation/reference_driver.h"

#include <components/camera_component.h>
#include <components/mesh_component.h>

#include <imgui.h>

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
        Layer::update();
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
        constexpr float VehicleYawOffset = -90.0f;
        constexpr float VehicleScale = 0.0015f;
        for (const auto& agent : m_agents)
        {
            aiko::Transform transform;
            const auto& car = agent.car;
            transform.position = car.position();
            transform.rotation = { 0.0f, -aiko::math::degrees(car.heading()) + VehicleYawOffset, 0.0f};
            transform.scale = {VehicleScale};
            renderer().drawModel(transform, m_vehicleModel);
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

        const  auto finishLine = m_track.getFinishLine();
        m_agents.clear();
        for (size_t i = 0 ; i < N_Vehicles; ++i)
        {
            m_agents.push_back({});
            const auto heading = std::atan2(finishLine.forward.z, finishLine.forward.x);

            auto& agent = m_agents.back();
            agent.car.reset(finishLine.position, heading);

            const auto projection = m_track.projectPoint(agent.car.position());

            agent.previousProgress = projection.progressOnTrack;
            agent.totalProgress = 0.0f;
            agent.bestProgress = 0.0f;

        }

        m_simulationAccumulator = 0.0f;

        return true;

    }

    void ApexBorn::simulateStep(float dt)
    {
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
                const auto control = ReferenceDriver::compute(car, m_track);
                car.setSteering(control.steering);
                car.setThrottle(control.throttle);
                car.setBrake(control.brake);
            }

            car.update(dt);

            const auto projection = m_track.projectPoint(car.position());

            if (!m_track.isOnRoad(projection))
            {
                agent.alive = false;
                continue;
            }

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

        ImGui::Checkbox("Autonomous Driving", &m_autonomousDriving);

        for (auto& agent : m_agents)
        {
            ImGui::PushID(&agent);
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

            //  Validation
            auto validation = DrivingObservation::calculate(car, m_track);
            ImGui::BeginDisabled();
            ImGui::SliderFloat("LateralOffset",  &validation.lateralOffset, -1.0f, 1.0f);
            ImGui::SliderFloat("headingError",   &validation.headingError, -1.0f, 1.0f);
            ImGui::SliderFloat("lookAheadError", &validation.lookAheadError, -1.0f, 1.0f);
            ImGui::EndDisabled();

            ImGui::PopID();
        }

        ImGui::End();
    }

}

