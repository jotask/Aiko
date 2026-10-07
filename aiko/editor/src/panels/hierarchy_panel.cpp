#include "hierarchy_panel.h"

#include "commands/game_object/create_game_object_command.h"
#include "commands/game_object/reparent_game_object_command.h"
#include "commands/game_object/rename_game_object_command.h"
#include "commands/game_object/duplicate_game_object_command.h"
#include "commands/game_object/delete_game_object_command.h"
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

                // Empty-space target: dropping here makes the object a root object.
                // It also owns the hierarchy background context menu.
                ImVec2 avail = ImGui::GetContentRegionAvail();
                if (avail.y > 0.0f)
                {
                    ImGui::InvisibleButton("HierarchyRootDropTarget", avail);

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_GAMEOBJECT"))
                        {
                            GameObject* draggedObject = *static_cast<GameObject* const*>(payload->Data);

                            detachFromParent(context, draggedObject);
                        }

                        ImGui::EndDragDropTarget();
                    }

                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Create GameObject"))
                        {
                            CreateGameObjectCommand& command = context.commands().execute<CreateGameObjectCommand>(sceneSystem);
                            context.select(command.createdObject());
                        }

                        ImGui::EndPopup();
                    }
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
                    if (!m_renameBuffer.empty() && m_renameBuffer != obj->getName())
                    {
                        context.commands().execute<RenameGameObjectCommand>(context.sceneSystem(), *obj, m_renameBuffer);
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
                            attachChild(context, obj, draggedObject);
                        }
                    }

                    ImGui::EndDragDropTarget();
                }

                if (ImGui::BeginPopupContextItem())
                {
                    context.select(obj);

                    if (ImGui::MenuItem("Create Child GameObject"))
                    {
                        CreateGameObjectCommand& command = context.commands().execute<CreateGameObjectCommand>(context.sceneSystem(), obj);
                        context.select(command.createdObject());

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

                    if (ImGui::MenuItem("Duplicate"))
                    {
                        DuplicateGameObjectCommand& command = context.commands().execute<DuplicateGameObjectCommand>(context.sceneSystem(), *obj);

                        context.select(command.duplicatedObject());

                        ImGui::EndPopup();

                        if (opened)
                        {
                            ImGui::TreePop();
                        }

                        return;
                    }

                    if (ImGui::MenuItem("Delete"))
                    {
                        if (context.selectedGameObject() == obj)
                        {
                            context.clearSelection();
                        }

                        context.commands().execute<DeleteGameObjectCommand>(context.sceneSystem(), *obj);

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

        void HierarchyPanel::attachChild(EditorContext& context, GameObject* parent, GameObject* child)
        {
            if (parent == nullptr || child == nullptr)
            {
                return;
            }

            if (child->transform().getParent() == &parent->transform())
            {
                return;
            }

            context.commands().execute<ReparentGameObjectCommand>(context.sceneSystem(), *child, parent);
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

        void HierarchyPanel::detachFromParent(EditorContext& context, GameObject* child)
        {
            if (child == nullptr)
            {
                return;
            }

            if (child->transform().getParent() == nullptr)
            {
                return;
            }

            context.commands().execute<ReparentGameObjectCommand>(context.sceneSystem(), *child, nullptr);
        }
    }
}
