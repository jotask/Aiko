#include "inspector_panel.h"

#include "registry/component_registry.h"
#include "core/editor_context.h"
#include "core/imgui_helper.h"
#include "registry/components_functionality.h"
#include "registry/components_render.h"
#include "commands/game_object/rename_game_object_command.h"
#include "commands/transform/transform_command.h"
#include <components/transform_component.h>
#include <models/game_object.h>
#include <systems/scene_system.h>

#include <aiko_includes.h>
#include <imgui.h>

namespace aiko
{
    namespace editor
    {

        namespace
        {
            TransformState captureTransform(const Transform& transform)
            {
                return
                {
                    transform.position,
                    transform.rotation,
                    transform.scale
                };
            }
        }

        InspectorPanel::InspectorPanel()
            : EditorPanel("Inspector")
        {
        }

        void InspectorPanel::finishTransformEdit(EditorContext& context, const TransformState& after)
        {
            if (m_transformEditObjectId)
            {
                GameObject* object = context.sceneSystem().findGameObject(*m_transformEditObjectId);

                if (object == m_transformEditObject && !(m_transformEditBefore == after))
                {
                    context.commands().pushExecuted<TransformCommand>(context.sceneSystem(), *m_transformEditObjectId, m_transformEditBefore, after);
                }
            }

            m_transformEditObjectId.reset();
            m_transformEditObject = nullptr;
        }


        void InspectorPanel::render(EditorContext& context)
        {
            GameObject* selectedGameObject = context.selectedGameObject();

            if (m_transformEditObjectId && selectedGameObject != m_transformEditObject)
            {
                GameObject* previous = context.sceneSystem().findGameObject(*m_transformEditObjectId);

                if (previous == m_transformEditObject)
                {
                    finishTransformEdit(context, captureTransform(previous->transform()));
                }
                else
                {
                    m_transformEditObjectId.reset();
                    m_transformEditObject = nullptr;
                }
            }

            bool transformRendered = false;

            vector<Component*> componentsToRemove;
            if (ImGui::Begin("Inspector"))
            {
                if (selectedGameObject != nullptr)
                {
                    ImGui::Text("Uuid: %s", selectedGameObject->uuid().get().c_str() );
                    string name = selectedGameObject->getName();

                    imgui::InputText("Name", &name);

                    if (ImGui::IsItemDeactivatedAfterEdit())
                    {
                        if (name.empty() == false && name != selectedGameObject->getName())
                        {
                            context.commands().execute<RenameGameObjectCommand>(context.sceneSystem(), *selectedGameObject, name);
                        }
                    }
                    ImGui::Spacing();
                    ImGui::Spacing();
                    for (Component* comp : selectedGameObject->getComponents())
                    {
                        if (ImGui::CollapsingHeader(comp->getName(), ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            const component::ComponentEditorEntry* entry = component::findComponentEntry(*comp);
                            const bool removable = entry != nullptr && entry->removable;
                            ImGui::PushID(comp);

                            if (removable && ImGui::Button("Remove"))
                            {
                                componentsToRemove.push_back(comp);
                                ImGui::PopID();
                                continue;
                            }
                            ImGui::PopID();

                            bool changed = false;

                            if (comp == selectedGameObject->getComponent<TransformComponent>())
                            {
                                transformRendered = true;

                                const TransformState beforeDraw = captureTransform(selectedGameObject->transform());

                                ImGui::BeginGroup();
                                changed = component::drawComponent(context, comp);
                                ImGui::EndGroup();

                                const bool deactivated = ImGui::IsItemDeactivated();
                                const bool activated = ImGui::IsItemActivated();

                                if (deactivated && m_transformEditObjectId && m_transformEditObject == selectedGameObject)
                                {
                                    const TransformState after = activated ? beforeDraw : captureTransform(selectedGameObject->transform());

                                    finishTransformEdit(context, after);
                                }

                                if (activated)
                                {
                                    m_transformEditObjectId = selectedGameObject->uuid();
                                    m_transformEditObject = selectedGameObject;
                                    m_transformEditBefore = beforeDraw;
                                }
                            }
                            else
                            {
                                changed = component::drawComponent(context, comp);
                            }

                            if (changed)
                            {
                                context.document().markDirty();
                            }

                        }
                    }
                    ImGui::Spacing();
                    ImGui::Spacing();
                    if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 0)) == true)
                    {
                        ImGui::OpenPopup("Add Component Context");
                    }
                    if (ImGui::BeginPopup("Add Component Context"))
                    {
                        ImGui::SeparatorText("Aquarium");

                        static ImGuiTextFilter filter;
                        ImGui::Text("Filter usage:\n"
                            "  \"\"         display all lines\n"
                            "  \"xxx\"      display lines containing \"xxx\"\n"
                            "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
                            "  \"-xxx\"     hide lines containing \"xxx\"");
                        filter.Draw();
                        vector<string> components = component::getMissingComponents(selectedGameObject);
                        for(string component : components)
                        {
                            if (filter.PassFilter(component.c_str()))
                            {
                                if (ImGui::Selectable(component.c_str()) == true)
                                {
                                    component::addComponent(context, component, *selectedGameObject);
                                }
                            }
                        }
                        ImGui::EndPopup();
                    }
                }
                else
                {
                    ImGui::Text("Nothing selected");
                }
            }
            ImGui::End();

            if (selectedGameObject != nullptr)
            {
                for (Component* component : componentsToRemove)
                {
                    if (component != nullptr)
                    {
                        component::removeComponent(context, *component);
                    }
                }
            }

            if (m_transformEditObjectId && !transformRendered)
            {
                GameObject* object = context.sceneSystem().findGameObject(*m_transformEditObjectId);

                if (object == m_transformEditObject)
                {
                    finishTransformEdit(context, captureTransform(object->transform()));
                }
                else
                {
                    m_transformEditObjectId.reset();
                    m_transformEditObject = nullptr;
                }
            }

        }

    }
}
