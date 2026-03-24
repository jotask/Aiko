#include "hirearchy_window.h"

#include "window.h"

#include <aiko_includes.h>
#include "events/editor_events.h"
#include "aiko_editor.h"
#include <systems/scene_system.h>

#include <imgui.h>

namespace aiko
{
    namespace editor
    {

        HirearchyWindow::HirearchyWindow(AikoEditor* editor)
            : Window(editor, "HirearchyWindow")
        {
        }

        void HirearchyWindow::init()
        {
        }

        void HirearchyWindow::render()
        {
            static auto* ecs = m_editor->getAiko()->getSystem<SceneSystem>();
            static GameObject* selectedGo = nullptr;

            static const auto selectGo = [&](GameObject* ptr)
            {
                selectedGo = ptr;
                HirearchyGameObjectSelectedEvent ev(selectedGo);
                EventSystem::it().sendEvent(ev);
            };

            if (ImGui::Begin("Hirearchy"))
            {
                static bool initialOpenState = true;
                ImGui::SetNextItemOpen(initialOpenState);
                if (ImGui::TreeNode("Scene"))
                {
                    Scene& scene = ecs->getScene();
                    for (GameObject* child : scene.getObjects())
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
                                scene.remove(child);
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
