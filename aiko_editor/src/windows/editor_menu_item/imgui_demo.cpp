#include "imgui_demo.h"

#include <imgui.h>

namespace aiko
{
    namespace editor
    {

        ImGuiDemo::ImGuiDemo(AikoEditor* editor)
            : MenuItem(editor, "ImGuiDemo")
        {

        }

        void ImGuiDemo::render()
        {
            ImGui::ShowDemoWindow(&is_open);
        }

    }
}
