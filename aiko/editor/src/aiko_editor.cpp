#include "aiko_editor.h"

#include "components/camera_component.h"
#include "components/light_component.h"
#include "core/editor_style.h"
#include "panels/game_view_panel.h"
#include "panels/hierarchy_panel.h"
#include "panels/inspector_panel.h"
#include "panels/main_menu_bar.h"
#include "panels/scene_view_panel.h"
#include "systems/render_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"

#include <display/display_events.hpp>
#include <events/events.hpp>

#include <imgui.h>
#include <ImGuizmo.h>

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
        m_workspace.addPanel<SceneViewPanel>();
        m_workspace.addPanel<GameViewPanel>();
        m_workspace.addPanel<HierarchyPanel>();
        m_workspace.addPanel<InspectorPanel>();
        menuBar.setWorkspace(&m_workspace);


    }

    void AikoEditor::render()
    {
        ImGuizmo::BeginFrame();
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::DockSpaceOverViewport(viewport->ID);
        applyViewSettings();
        renderSceneGizmos();
        m_workspace.render(m_context);
    }

    void AikoEditor::applyViewSettings()
    {
        const EditorViewSettings& settings = m_context.viewSettings();
        if (settings.overrideClearColor)
        {
            m_context.renderSystem().setClearColor(settings.clearColor);
            return;
        }
        const Scene& scene = m_context.sceneSystem().getScene();
        m_context.renderSystem().setClearColor(scene.clearColor());
    }

    void AikoEditor::renderSceneGizmos()
    {
        const EditorViewSettings& settings = m_context.viewSettings();

        Scene& scene = m_context.sceneSystem().getScene();

        if (settings.showLightGizmos)
        {
            for (LightComponent* light : scene.components<LightComponent>())
            {
                if (light == nullptr || light->isActiveAndEnabled() == false)
                {
                    continue;
                }

                GameObject* object = light->getGameObject();

                AIKO_ASSERT(object != nullptr, "LightComponent is not attached to a GameObject");

                m_context.renderSystem().renderLightGizmo(object->transform().position, vec3(0.15f), light->color);
            }
        }

        if (settings.showCameraGizmos)
        {
            for (CameraComponent* cameraComponent : scene.components<CameraComponent>())
            {
                if (cameraComponent == nullptr || cameraComponent->isActiveAndEnabled() == false)
                {
                    continue;
                }

                m_context.renderSystem().renderCameraGizmo(cameraComponent->getCamera(), Color(0.9f, 0.9f, 0.25f, 1.0f));
            }
        }
    }

}