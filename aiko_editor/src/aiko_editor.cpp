#include "aiko_editor.h"

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"

#include "windows/game_window.h"
#include "windows/hirearchy_window.h"

// #include <imgui.h>

namespace aiko::editor
{

    aiko::Aiko* AikoEditor::getAiko() const
    {
        return m_aiko.get();
    }

    void AikoEditor::init()
    {
        Application::init();
        /*
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        m_windows.emplace_back(std::make_unique<GameWindow>(this));
        m_windows.emplace_back(std::make_unique<HirearchyWindow>(this));
        */

    }

    void AikoEditor::render()
    {
        /*
        // Menu Bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                    // Trigger file open dialog or handle opening files
                }
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    // Handle saving current work
                }
                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    // Handle exiting the application
                    getAiko()->close();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                    // Handle undo
                }
                // More edit options here
                ImGui::EndMenu();
            }
            // Add more menus as needed

            ImGui::EndMainMenuBar();
        }

        // Docking Space// Docking Space
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        for (auto& tmp : m_windows)
        {
            tmp->render();
        }
        */

    }
}

