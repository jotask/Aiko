#include "aiko_editor.h"

#include "components/camera_component.h"
#include "components/light_component.h"
#include "core/editor_style.h"
#include "panels/game_view_panel.h"
#include "panels/hierarchy_panel.h"
#include "panels/inspector_panel.h"
#include "panels/main_menu_bar.h"
#include "panels/scene_view_panel.h"
#include "scene/scene_bounds.h"
#include "systems/asset_system.h"
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
        BIND_SYSTEM_REQUIRED_REF(AssetSystem, connector, m_assetSystem);
    }

    void AikoEditor::init()
    {
        AIKO_ASSERT(m_renderSystem != nullptr, "Editor requires RenderSystem");
        AIKO_ASSERT(m_sceneSystem != nullptr, "Editor requires SceneSystem");
        AIKO_ASSERT(m_assetSystem != nullptr, "Editor requires AssetSystem");

        m_context.connect(*m_renderSystem, *m_sceneSystem, *m_assetSystem);

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
        menuBar.setRuntime(&runtime());

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

                const bool selectedLight = object == m_context.selectedGameObject();

                const Color color = selectedLight ? Color(1.0f, 0.6f, 0.1f, 1.0f) : light->color;

                m_context.renderSystem().renderLightGizmo(object->transform().position, vec3(0.15f), color);
            }
        }

        if (settings.showCameraGizmos)
        {
            GameObject* selected = m_context.selectedGameObject();

            for (CameraComponent* cameraComponent : scene.components<CameraComponent>())
            {
                if (cameraComponent == nullptr || cameraComponent->isActiveAndEnabled() == false)
                {
                    continue;
                }

                GameObject* object = cameraComponent->getGameObject();

                AIKO_ASSERT(object != nullptr, "CameraComponent is not attached to a GameObject");

                const bool selectedCamera = object == selected;

                const Color color = selectedCamera ? Color(1.0f, 0.6f, 0.1f, 1.0f) : Color(0.9f, 0.9f, 0.25f, 1.0f);

                m_context.renderSystem().renderCameraGizmo(cameraComponent->getCamera(), color);
            }
        }

        if (settings.showBounds)
        {
            for (GameObject* object : scene.getObjects())
            {
                if (object == nullptr || !object->isActiveInHierarchy())
                {
                    continue;
                }

                Bounds bounds;

                if (!calculateSceneObjectBounds(m_context, *object, bounds))
                {
                    continue;
                }

                const bool selected = object == m_context.selectedGameObject();

                const Color color =
                    selected
                        ? Color(1.0f, 0.6f, 0.1f, 1.0f)
                        : Color(0.2f, 0.8f, 1.0f, 1.0f);

                m_context.renderSystem().renderBoundsGizmo(bounds, color);
            }
        }

    }

}
