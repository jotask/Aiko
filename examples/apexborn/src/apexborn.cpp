#include "apexborn.h"

#include "layers/contexts/scene_context.h"
#include "models/mesh_factory.h"

#include <components/camera_component.h>
#include <components/mesh_component.h>

#include <imgui.h>

namespace ab
{

    void ApexBorn::init()
    {

        scene().clearColor() = aiko::BLACK;

        aiko::CameraComponent* camera = scene().createCamera(aiko::camera::CameraController::Fly, aiko::Camera::CameraType::Perspective);
        camera->getCamera().position = { 0.0f, 8.0f, 10.0f };
        camera->getCamera().target = {0.0f, 0.0f, 0.0f};

        m_track.generate();

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
            ImGui::EndMainMenuBar();
        }
    }

}

