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
            if (ImGui::Begin("Hirearchy"))
            {
                static bool initialOpenState = true;
                ImGui::SetNextItemOpen(initialOpenState);
                if (ImGui::TreeNode("Scene"))
                {
                    if(ImGui::TreeNode("Main Camera"))
                    {
                        aiko::Camera* camera = m_editor->getAiko()->getSystem<aiko::CameraSystem>()->getMainCamera();
                        ::editor::component::drawCamera(camera);
                        ImGui::TreePop();
                    }

                    for (GameObject* child : ecs->getObjects())
                    {
                        const bool isSelected = (child == selectedGo);
                        if (ImGui::Selectable(child->getName().c_str(), isSelected))
                        {
                            selectedGo = child;
                            aiko::EventSystem::it().sendEvent(::editor::HirearchyGameObjectSelectedEvent(selectedGo));
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
                                    selectedGo = nullptr;
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
                        ecs->createGameObject();
                    }
                    ImGui::EndPopup();
                }

                // Check for left-click on the background of the window
                if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    selectedGo = nullptr;
                    aiko::EventSystem::it().sendEvent(::editor::HirearchyGameObjectUnSelectedEvent());
                }

            }
            ImGui::End();
        }

    }
}
