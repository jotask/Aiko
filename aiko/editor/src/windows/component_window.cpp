#include "component_window.h"

#include "window.h"

#include <aiko_includes.h>
#include "registry/components_render.h"
#include "registry/components_functionality.h"
#include "aiko_editor.h"
#include "core/imgui_helper.h"

#include <imgui.h>

namespace aiko
{
    namespace editor
    {

        ComponentWindow::ComponentWindow(AikoEditor* editor)
            : Window(editor, "ComponentWindow")
        {
        }

        void ComponentWindow::init()
        {
        }

        void ComponentWindow::render()
        {
            GameObject* selectedGameObject = context().getSelectedGameObject();
            vector<Component*> componentsToRemove;
            if (ImGui::Begin("Components"))
            {
                if (selectedGameObject != nullptr)
                {
                    ImGui::Text("Uuid: %s", selectedGameObject->uuid().get().c_str() );
                    string name = selectedGameObject->getName();
                    if (imgui::InputText("Name", &name))
                    {
                        selectedGameObject->setName(name);
                    }
                    ImGui::Spacing();
                    ImGui::Spacing();
                    for (Component* comp : selectedGameObject->getComponents())
                    {
                        if (ImGui::CollapsingHeader(comp->getName(), ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            ImGui::PushID(comp);
                            if (ImGui::Button("Remove") == true)
                            {
                                componentsToRemove.push_back(comp);
                                ImGui::PopID();
                                continue;
                            }
                            ImGui::PopID();
                            component::drawComponent(comp);
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
                                    component::addComponent(component, selectedGameObject);
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
                for(Component* cmp : componentsToRemove)
                {
                    selectedGameObject->removeComponent(cmp);
                }
            }

        }

    }
}
