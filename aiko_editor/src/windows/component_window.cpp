#include "component_window.h"

#include "window.h"

#include <aiko_includes.h>
#include "events/editor_events.h"
#include "core/components_render.h"
#include "aiko_editor.h"

#include <imgui.h>

namespace aiko
{
    namespace editor
    {

        ComponentWindow::ComponentWindow(AikoEditor* editor)
            : Window(editor)
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
                    ImGui::Text(selectedGo->getName().c_str());
                    for (auto* comp : selectedGo->getComponents())
                    {
                        if (ImGui::CollapsingHeader(comp->getName()))
                        {
                            ::editor::component::drawComponent(comp);
                        }
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
