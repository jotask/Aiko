#include "apexborn.h"

#include "layers/contexts/scene_context.h"
#include "models/mesh_factory.h"

#include <components/camera_component.h>
#include <components/mesh_component.h>

#include "extensions/ImGuiRoadProfiler.h"

#include <imgui.h>

namespace ab
{

    ApexBorn::ApexBorn() = default;

    ApexBorn::~ApexBorn() = default;

    void ApexBorn::init()
    {

        scene().clearColor() = aiko::BLACK;

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly, aiko::Camera::CameraType::Perspective);
        camera->getCamera().position = { 0.0f, 50.0f, 10.0f };
        camera->getCamera().target = {0.0f, 0.0f, 0.0f};

        m_track.init(assets());
        const bool generated = m_track.generate(m_trackSettings);
        if (generated == false)
        {
            aiko::logger::Log::error("Couldn't generate track with default settings");
        }

    }

    void ApexBorn::update()
    {
        Layer::update();
    }

    void ApexBorn::render()
    {
        Layer::render();
        renderImgui();
        m_track.render(renderer());
    }

    void ApexBorn::renderImgui()
    {
        renderMenuBar();
        if (m_trackImgui.isOpen)
        {
            renderTrackGenerationSettings();
        }
        if (m_trackImgui.isRoadProfilerEditor)
        {
            ImGuiRoadProfiler::Render(&m_trackImgui.isRoadProfilerEditor);
        }
    }

    void ApexBorn::renderMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::BeginMenu("Open"))
                {
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Close All Documents", NULL, false))
                {

                }
                if (ImGui::MenuItem("Exit"))
                {

                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools"))
            {
                ImGui::MenuItem("RoadGenerationSettings", nullptr, &m_trackImgui.isOpen);
                ImGui::MenuItem("RoadProfilerEditor", nullptr, &m_trackImgui.isRoadProfilerEditor);
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
            const bool generated = m_track.generate(m_trackSettings);
            if (generated == false)
            {
                aiko::logger::Log::error("Couldn't generate track with current settings");
            }
        }

        ImGui::End();
    }

}

