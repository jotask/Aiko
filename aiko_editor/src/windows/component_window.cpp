#include "component_window.h"

#include "window.h"

#include <aiko_includes.h>
#include "events/editor_events.h"
#include "core/components_render.h"
#include "aiko_editor.h"
#include "core/imgui_helper.h"

#include <imgui.h>

namespace aiko
{
    namespace editor
    {

        ComponentWindow::ComponentWindow(AikoEditor* editor)
            : Window(editor, "ComponentWindow")
            , selectedGo(nullptr)
        {
            aiko::EventSystem::it().bind<::editor::HirearchyGameObjectSelectedEvent>(this, &aiko::editor::ComponentWindow::onGameObjectSelected);
            aiko::EventSystem::it().bind<::editor::HirearchyGameObjectUnSelectedEvent>(this, &aiko::editor::ComponentWindow::onGameObjectUnSelected);
        }

        void ComponentWindow::render()
        {
            static auto* ecs = m_editor->getAiko()->getSystem<aiko::EntityComponentSystem>();
            if (ImGui::Begin("Components"))
            {
                if (selectedGo != nullptr)
                {
                    std::string name = selectedGo->getName();
                    if (::editor::ImGui::InputText("Name", &name))
                    {
                        selectedGo->setName(name);
                    }
                    ImGui::Spacing();
                    ImGui::Spacing();
                    for (Component* comp : selectedGo->getComponents())
                    {
                        if (ImGui::CollapsingHeader(comp->getName(), ImGuiTreeNodeFlags_DefaultOpen))
                        {
                            ImGui::PushID(comp);
                            if (ImGui::Button("Remove") == true)
                            {
                                selectedGo->removeComponent(comp);
                                ImGui::PopID();
                                continue;
                            }
                            ImGui::PopID();
                            ::editor::component::drawComponent(comp);
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
                        std::vector<std::string> components = ::editor::component::getMissingComponents(selectedGo);
                        for(std::string component : components)
                        {
                            if (filter.PassFilter(component.c_str()))
                            {
                                if (ImGui::Selectable(component.c_str()) == true)
                                {
                                    ::editor::component::addComponent(component, selectedGo);
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

        }

        void ComponentWindow::onGameObjectSelected(aiko::Event& envt)
        {
            const auto& msg = static_cast<const ::editor::HirearchyGameObjectSelectedEvent&>(envt);
            selectedGo = msg.selected;
        }

        void ComponentWindow::onGameObjectUnSelected(aiko::Event& envt)
        {
            const auto& msg = static_cast<const ::editor::HirearchyGameObjectUnSelectedEvent&>(envt);
            selectedGo = nullptr;
        }

    }
}
