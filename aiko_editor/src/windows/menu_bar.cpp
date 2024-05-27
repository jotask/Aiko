#include "menu_bar.h"

#include "aiko_editor.h"
#include "aiko.h"
#include "systems/render_system.h"

#include "window.h"

#include "editor_menu_item/asset_manager.h"

#include <imgui.h>

namespace aiko
{
    namespace editor
    {

        MenuBar::MenuBar(AikoEditor* editor)
            : Window(editor, "MenuBar")
        {
            m_items.emplace_back(std::make_unique<AssetManager>(editor));
        }

        void MenuBar::render()
        {
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
                        m_editor->getAiko()->close();
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
                if (ImGui::BeginMenu("Editor"))
                {
                    for (auto& tmp : m_editor->getWindows())
                    {
                        if (tmp.get() == this)
                        {
                            continue;
                        }
                        if (ImGui::MenuItem(tmp->getName(), nullptr, &tmp->is_open))
                        {
                            // Handle undo
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        }

    }
}
