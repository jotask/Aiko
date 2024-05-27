#include "aiko_editor.h"

#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"

#include "windows/game_window.h"
#include "windows/hirearchy_window.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace aiko::editor
{

    aiko::Aiko* AikoEditor::getAiko() const
    {
        return m_aiko.get();
    }

    void AikoEditor::init()
    {
        // https://www.codingwiththomas.com/blog/rendering-an-opengl-framebuffer-into-a-dear-imgui-window
        Application::init();
        
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        m_windows.emplace_back(std::make_unique<GameWindow>(this));
        m_windows.emplace_back(std::make_unique<HirearchyWindow>(this));

    }

    void AikoEditor::render()
    {
        // Docking Space// Docking Space
        auto main_viewport = ImGui::GetMainViewport();
        ImGui::DockSpaceOverViewport(main_viewport->ID);

        for (auto& tmp : m_windows)
        {
            tmp->render();
        }

    }
}

