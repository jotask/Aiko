#include "hierarchy_panel.h"

#include "core/editor_context.h"
#include "core/imgui_helper.h"

#include <systems/scene_system.h>

#include <aiko_includes.h>
#include <algorithm>
#include <cfloat>
#include <imgui.h>

namespace aiko
{
    namespace editor
    {

        HierarchyPanel::HierarchyPanel()
            : EditorPanel("Hierarchy")
        {
        }

        void HierarchyPanel::render(EditorContext& context)
        {
            SceneSystem& sceneSystem = context.sceneSystem();
            if (ImGui::Begin("Hierarchy"))
            {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::TreeNode("Scene"))
                {
                    Scene& scene = sceneSystem.getScene();
                    for (GameObject* child : scene.getObjects())
                    {
                        if (child == nullptr)
                        {
                            continue;
                        }

                        if (child->transform().getParent() == nullptr)
                        {
                            renderGameObject(scene, child, context);
                        }
                    }
                    ImGui::TreePop();
                }

                // Empty-space drop target: dropping here makes the object a root object.
                ImVec2 avail = ImGui::GetContentRegionAvail();
                if (avail.y > 0.0f)
                {
                    ImGui::InvisibleButton("HierarchyRootDropTarget", avail);

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_GAMEOBJECT"))
                        {
                            GameObject* draggedObject = *static_cast<GameObject* const*>(payload->Data);
                            detachFromParent(draggedObject);
                        }

                        ImGui::EndDragDropTarget();
                    }
                }

                if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
                {
                    if (ImGui::MenuItem("Create GameObject"))
                    {
                        GameObject* go = sceneSystem.createGameObject();
                        context.select(go);
                    }
                    ImGui::EndPopup();
                }

                // Check for left-click on the background of the window
                if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) == true
                    && ImGui::IsAnyItemHovered() == false
                    && ImGui::IsMouseClicked(ImGuiMouseButton_Left) ==  true)
                {
                    context.select(nullptr);
                }

            }
            ImGui::End();
        }

        void HierarchyPanel::renderGameObject(Scene& scene, GameObject* obj, EditorContext& context)
        {
            if (obj == nullptr)
            {
                return;
            }

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

            if (context.selectedGameObject() == obj)
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            if (obj->transform().getChildren().empty() == true)
            {
                flags |= ImGuiTreeNodeFlags_Leaf;
            }

            bool opened = false;

            if (m_renameTarget == obj)
            {
                opened = ImGui::TreeNodeEx(obj, flags, "%s", "");

                ImGui::SameLine(0.0f, 4.0f);
                ImGui::SetNextItemWidth(-FLT_MIN);

                if (m_renameJustStarted == obj)
                {
                    ImGui::SetKeyboardFocusHere();
                    m_renameJustStarted = nullptr;
                }

                imgui::InputText("##RenameGameObject", &m_renameBuffer, ImGuiInputTextFlags_EnterReturnsTrue);

                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    if (!m_renameBuffer.empty())
                    {
                        obj->setName(m_renameBuffer);
                    }
                    m_renameTarget = nullptr;
                }
            }
            else
            {
                opened = ImGui::TreeNodeEx(obj, flags, "%s", obj->getName().c_str());
            }

            if (m_renameTarget != obj)
            {
                if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
                {
                    context.select(obj);
                }

                if (ImGui::BeginDragDropSource())
                {
                    GameObject* payloadObject = obj;
                    ImGui::SetDragDropPayload("HIERARCHY_GAMEOBJECT", &payloadObject, sizeof(GameObject*));
                    ImGui::Text("%s", obj->getName().c_str());
                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_GAMEOBJECT"))
                    {
                        GameObject* draggedObject = *static_cast<GameObject* const*>(payload->Data);

                        if (canAttachChild(obj, draggedObject))
                        {
                            attachChild(obj, draggedObject);
                        }
                    }

                    ImGui::EndDragDropTarget();
                }

                if (ImGui::BeginPopupContextItem())
                {
                    context.select(obj);

                    if (ImGui::MenuItem("Create Child GameObject"))
                    {
                        GameObject* go = context.sceneSystem().createGameObject(obj);
                        context.select(go);

                        ImGui::EndPopup();

                        if (opened)
                        {
                            ImGui::TreePop();
                        }
                        return;
                    }

                    if (ImGui::MenuItem("Rename"))
                    {
                        m_renameTarget = obj;
                        m_renameJustStarted = obj;
                        m_renameBuffer = obj->getName();

                        ImGui::EndPopup();

                        if (opened)
                        {
                            ImGui::TreePop();
                        }
                        return;
                    }

                    if (ImGui::MenuItem("Delete"))
                    {
                        scene.remove(obj);

                        if (context.selectedGameObject() == obj)
                        {
                            context.clearSelection();
                        }

                        ImGui::EndPopup();

                        if (opened)
                        {
                            ImGui::TreePop();
                        }
                        return;
                    }

                    ImGui::EndPopup();
                }
            }

            if (opened == true)
            {
                for (GameObject* child : obj->getChildren())
                {
                    renderGameObject(scene, child, context);
                }
                ImGui::TreePop();
            }
        }

        void HierarchyPanel::attachChild(GameObject* parent, GameObject* child)
        {
            if (parent == nullptr || child == nullptr)
            {
                return;
            }

            Transform& parentTransform = parent->transform();
            Transform& childTransform = child->transform();

            if (childTransform.getParent() == &parentTransform)
            {
                return;
            }

            if (childTransform.getParent() != nullptr)
            {
                // FIXME
                // auto& siblings = childTransform.getParent()->getChildren();
                // siblings.erase(std::remove(siblings.begin(), siblings.end(), &childTransform), siblings.end());
            }

            childTransform.setParent(&parentTransform);
        }

        bool HierarchyPanel::canAttachChild(GameObject* parent, GameObject* child) const
        {
            if (parent == nullptr || child == nullptr)
            {
                return false;
            }

            if (parent == child)
            {
                return false;
            }

            Transform* current = &parent->transform();
            while (current != nullptr)
            {
                if (current == &child->transform())
                {
                    return false;
                }
                current = current->getParent();
            }
            return true;
        }

        void HierarchyPanel::detachFromParent(GameObject* child)
        {
            if (child == nullptr)
            {
                return;
            }

            Transform& childTransform = child->transform();

            if (childTransform.getParent() != nullptr)
            {
                // FIXME
                // auto& siblings = childTransform.getParent()->getChildren();
                // siblings.erase(std::remove(siblings.begin(), siblings.end(), &childTransform), siblings.end());
                childTransform.setParent(nullptr);
            }
        }
    }
}
