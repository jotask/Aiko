#include "hirearchy_window.h"

#include "window.h"

#include <aiko_includes.h>
#include "events/editor_events.h"
#include "aiko_editor.h"
#include "core/components_render.h"

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
            static GameObject* selectedGo = nullptr;

            static constexpr const auto selectGo = [&](GameObject* ptr)
            {
                selectedGo = ptr;
                aiko::EventSystem::it().sendEvent(::editor::HirearchyGameObjectSelectedEvent(selectedGo));
            };

            if (ImGui::Begin("Hirearchy"))
            {
                static bool initialOpenState = true;
                ImGui::SetNextItemOpen(initialOpenState);
                if (ImGui::TreeNode("Scene"))
                {
                    for (GameObject* child : ecs->getObjects())
                    {
                        const bool isSelected = (child == selectedGo);
                        if (ImGui::Selectable(child->getName().c_str(), isSelected))
                        {
                            selectGo(child);
                        }

                        // Context menu for right-click
                        if (ImGui::BeginPopupContextItem(child->getName().c_str()))
                        {
                            if (ImGui::MenuItem("Delete"))
                            {
                                // Handle deletion of the GameObject
                                ecs->destroyGameObject(child);
                                if (selectedGo == child)
                                {
                                    selectGo(nullptr);
                                }
                            }
                            ImGui::EndPopup();
                        }

                    }
                    ImGui::TreePop();
                }
                if (ImGui::BeginPopupContextWindow())
                {
                    if (ImGui::MenuItem("Create GameObject"))
                    {
                        aiko::GameObject* go = ecs->createGameObject().get();
                        selectGo(go);
                    }
                    ImGui::EndPopup();
                }

                // Check for left-click on the background of the window
                if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    selectGo(nullptr);
                }

            }
            ImGui::End();
        }

    }
}
