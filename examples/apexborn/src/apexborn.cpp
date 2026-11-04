#include "apexborn.h"

#include "extensions/ImGuiRoadProfiler.h"
#include "layers/contexts/asset_context.h"
#include "layers/contexts/scene_context.h"
#include "models/mesh_factory.h"

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

        loadVehicleModel();

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
        const auto dt = getDeltaTime();
        const auto trackLength = m_track.getTrackLength();
        for (auto& agent : m_agents)
        {
            if (agent.alive == false)
            {
                continue;
            }
            auto& car = agent.car;
            car.update(dt);
            const auto projection = m_track.projectPoint(car.position());
            if (m_track.isOnRoad(projection) == false)
            {
                agent.alive = false;
                continue;
            }

            float delta = projection.progressOnTrack - agent.previousProgress;

            if (delta < -trackLength * 0.5f)
            {
                delta += trackLength;
            }
            else if (delta > trackLength  * 0.5f)
            {
                delta -= trackLength;
            }
            agent.totalProgress += delta;
            agent.previousProgress = projection.progressOnTrack;
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
            for (const auto& part : m_vehicleParts)
            {
                renderer().drawMesh(transform, *part.mesh, part.material);
            }
        }

    }

    void ApexBorn::loadVehicleModel()
    {
        // Load vehicle model
        const auto importedModel = assets().importModel("porsche.fbx");
        AIKO_ASSERT(importedModel, "Failed to import Porsche model");
        AIKO_ASSERT(!importedModel->submeshes.empty(), "Porsche model contains no meshes");

        m_vehicleParts.clear();
        m_vehicleParts.reserve(importedModel->submeshes.size());

        for (const auto& importedPart : importedModel->submeshes)
        {
            auto& part = m_vehicleParts.emplace_back();

            part.mesh = std::make_unique<aiko::Mesh>();
            part.mesh->upload(importedPart.mesh);

            AIKO_ASSERT(part.mesh->isValid(), "Failed to upload Porsche submesh");

            part.material.m_shaderId = assets().loadShader(importedPart.shaderSource);
            part.material.m_useVertexColor = importedPart.useVertexColor;
            part.material.m_lit = importedPart.lit;
            part.material.m_baseColor = importedPart.baseColor;
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

        bool result =  m_environment.generate(groundSize);

        if (result == false)
        {
            return false;
        }

        constexpr float initialSpeed = 1.0f;
        const  auto finishLine = m_track.getFinishLine();
        m_agents.clear();
        for (size_t i = 0 ; i < N_Vehicles; ++i)
        {
            m_agents.push_back({});
            const auto heading = std::atan2(finishLine.forward.z, finishLine.forward.x);

            auto& agent = m_agents.back();
            agent.car.reset(finishLine.position, heading, initialSpeed);

            const auto projection = m_track.projectPoint(agent.car.position());

            agent.previousProgress = projection.progressOnTrack;
            agent.totalProgress = 0.0f;
            agent.numberOfLaps = 0;

        }

        return true;

    }

    void ApexBorn::renderImgui()
    {
        renderMenuBar();
        if (m_trackImgui.isOpen)
        {
            renderTrackGenerationSettings();
        }
        m_profileEditor.render(m_track.roadProfile());
        renderVehicleDebug();
    }

    void ApexBorn::renderMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Tools"))
            {
                ImGui::MenuItem("RoadGenerationSettings", nullptr, &m_trackImgui.isOpen);
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

        for (auto& agent : m_agents)
        {
            ImGui::PushID(&agent);
            auto& car = agent.car;
            const auto projection = m_track.projectPoint(car.position());
            ImGui::Text("Lateral Offset: %.2f", projection.signedLateralOffset);
            ImGui::Text("Track Progress: %.2f", projection.progressOnTrack);
            ImGui::Text("Total Progress: %.2f", agent.totalProgress);
            const float alignment = aiko::math::dot(car.forwardDirection(), projection.forwardDirection);
            ImGui::Text("Heading Alignment: %.3f", alignment);
            ImGui::TextColored(agent.alive ? ImVec4(0.2f, 0.9f, 0.3f, 1.0f) : ImVec4(0.9f, 0.2f, 0.2f, 1.0f), "%s", agent.alive ? "ACTIVE" : "FAILED");
            ImGui::BeginDisabled(!agent.alive);
            float currentSteering = car.steering();
            if (ImGui::SliderFloat("Steering", &currentSteering, -1.0f, 1.0f))
            {
                car.setSteering(currentSteering);
            }
            if (ImGui::Button("Reset Steering"))
            {
                car.setSteering(0.0f);
            }
            ImGui::EndDisabled();
            ImGui::PopID();
        }

        ImGui::End();
    }

}

