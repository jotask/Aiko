#include "aiko_editor.h"

#include <imgui.h>

#include <display/display_events.hpp>
#include <events/events.hpp>

#include "core/editor_style.h"

#include "panels/game_view_panel.h"
#include "panels/hierarchy_panel.h"
#include "panels/inspector_panel.h"
#include "panels/main_menu_bar.h"

#include "systems/render_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"
#include "components/camera_component.h"

namespace aiko::editor
{

    void AikoEditor::connect(SystemConnector& connector)
    {
        BIND_SYSTEM_REQUIRED_REF(RenderSystem, connector, m_renderSystem);
        BIND_SYSTEM_REQUIRED_REF(SceneSystem, connector, m_sceneSystem);
    }

    void AikoEditor::init()
    {
        AIKO_ASSERT(m_renderSystem != nullptr, "Editor requires RenderSystem");
        AIKO_ASSERT(m_sceneSystem != nullptr, "Editor requires SceneSystem");

        m_context.connect(*m_renderSystem, *m_sceneSystem);

        if (m_sceneSystem->getMainCamera() == nullptr)
        {
            GameObject* camera = m_sceneSystem->createGameObject("Camera");
            camera->addComponent<CameraComponent>(camera::CameraController::Orbit);
        }

        ImGuiIO& io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        applyEditorStyle();

        MainMenuBar& menuBar = m_workspace.addPanel<MainMenuBar>();
        m_workspace.addPanel<GameViewPanel>();
        m_workspace.addPanel<HierarchyPanel>();
        m_workspace.addPanel<InspectorPanel>();
        menuBar.setWorkspace(&m_workspace);


    }

    void AikoEditor::render()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::DockSpaceOverViewport(viewport->ID);

        m_workspace.render(m_context);
    }

}