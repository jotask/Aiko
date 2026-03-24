#include "hirearchy_window.h"

#include "window.h"
#include "aiko_editor.h"

#include <aiko_includes.h>
#include <systems/scene_system.h>

#include <imgui.h>

#include <algorithm>
#include <cfloat>

#include "core/imgui_helper.h"

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
            static auto* ecs = getAiko()->getSystem<SceneSystem>();

            if (ImGui::Begin("Hirearchy"))
            {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::TreeNode("Scene"))
                {
                    Scene& scene = ecs->getScene();
                    for (GameObject* child : scene.getObjects())
                    {
                        if (child == nullptr)
                        {
                            continue;
                        }

                        if (child->transform().parent == nullptr)
                        {
                            renderGameObject(scene, child);
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
                        AikoPtr<GameObject> go = ecs->createGameObject();
                        context().setSelectedGameObject(go.get());
                    }
                    ImGui::EndPopup();
                }

                // Check for left-click on the background of the window
                if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) == true
                    && ImGui::IsAnyItemHovered() == false
                    && ImGui::IsMouseClicked(ImGuiMouseButton_Left) ==  true)
                {
                    context().setSelectedGameObject(nullptr);
                }

            }
            ImGui::End();
        }

        void HirearchyWindow::renderGameObject(Scene& scene, GameObject* obj)
        {
            if (obj == nullptr)
            {
                return;
            }

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

            if (context().getSelectedGameObject() == obj)
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            if (obj->transform().childs.empty() == true)
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
                    context().setSelectedGameObject(obj);
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
                    context().setSelectedGameObject(obj);

                    if (ImGui::MenuItem("Create Child GameObject"))
                    {
                        auto* ecs = getAiko()->getSystem<SceneSystem>();
                        AikoPtr<GameObject> go = ecs->createGameObject();

                        attachChild(obj, go.get());
                        context().setSelectedGameObject(go.get());

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

                        if (context().getSelectedGameObject() == obj)
                        {
                            context().setSelectedGameObject(nullptr);
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
                for (Transform* childTransform : obj->transform().childs)
                {
                    if (childTransform == nullptr)
                    {
                        continue;
                    }

                    GameObject* childObject = findGameObjectByTransform(scene, childTransform);
                    renderGameObject(scene, childObject);
                }

                ImGui::TreePop();
            }
        }

        GameObject* HirearchyWindow::findGameObjectByTransform(Scene& scene, Transform* transform)
        {
            if (transform == nullptr)
            {
                return nullptr;
            }

            for (GameObject* obj : scene.getObjects())
            {
                if (obj == nullptr)
                {
                    continue;
                }

                if (&obj->transform() == transform)
                {
                    return obj;
                }
            }

            return nullptr;
        }

        void HirearchyWindow::attachChild(GameObject* parent, GameObject* child)
        {
            if (parent == nullptr || child == nullptr)
            {
                return;
            }

            Transform& parentTransform = parent->transform();
            Transform& childTransform = child->transform();

            if (childTransform.parent == &parentTransform)
            {
                return;
            }

            if (childTransform.parent != nullptr)
            {
                auto& siblings = childTransform.parent->childs;
                siblings.erase(std::remove(siblings.begin(), siblings.end(), &childTransform), siblings.end());
            }

            childTransform.parent = &parentTransform;
            parentTransform.childs.push_back(&childTransform);
        }

        bool HirearchyWindow::canAttachChild(GameObject* parent, GameObject* child) const
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
                current = current->parent;
            }
            return true;
        }

        void HirearchyWindow::detachFromParent(GameObject* child)
        {
            if (child == nullptr)
            {
                return;
            }

            Transform& childTransform = child->transform();

            if (childTransform.parent != nullptr)
            {
                auto& siblings = childTransform.parent->childs;
                siblings.erase(std::remove(siblings.begin(), siblings.end(), &childTransform), siblings.end());
                childTransform.parent = nullptr;
            }
        }
    }
}
