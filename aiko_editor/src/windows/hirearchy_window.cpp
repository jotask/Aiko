#include "hirearchy_window.h"

#include "window.h"

#include <aiko_includes.h>
#include "events/editor_events.h"
#include "aiko_editor.h"

#include <imgui.h>

namespace aiko
{
    namespace editor
    {

        HirearchyWindow::HirearchyWindow(AikoEditor* editor)
            : Window(editor, "HirearchyWindow")
        {

        }

        void HirearchyWindow::render()
        {
            static auto* ecs = m_editor->getAiko()->getSystem<aiko::EntityComponentSystem>();
            if (ImGui::Begin("Hirearchy"))
            {
                static bool initialOpenState = true;
                ImGui::SetNextItemOpen(initialOpenState);
                if (ImGui::TreeNode("Scene"))
                {
                    for (GameObject* child : ecs->getObjects())
                    {
                        if (ImGui::Selectable(child->getName().c_str()))
                        {
                            aiko::EventSystem::it().sendEvent(::editor::HirearchyGameObjectSelectedEvent(child));
                        }
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::End();
        }

    }
}
