#include "aiko_editor.h"

#include "core/editor_style.h"
#include "models/mesh_factory.h"
#include "windows/component_window.h"
#include "windows/game_window.h"
#include "windows/hirearchy_window.h"
#include "windows/menu_bar.h"
#include "systems/render_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"
#include <events/events.hpp>
#include <display/display_events.hpp>

#include <aiko_includes.h>
#include <imgui.h>

namespace aiko::editor
{
    AikoEditor::AikoEditor()
    {

    }

    void AikoEditor::connect(SystemConnector& connector)
    {
        BIND_SYSTEM_REQUIRED_REF(RenderSystem, connector, m_renderSystem);
        BIND_SYSTEM_REQUIRED_REF(SceneSystem, connector, m_sceneSystem);
    }

    void AikoEditor::init()
    {

        auto camera = Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(camera::CameraController::Orbit);

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        applyEditorStyle();

        m_windows.emplace_back(std::make_unique<MenuBar>(this));
        m_windows.emplace_back(std::make_unique<GameWindow>(this));
        m_windows.emplace_back(std::make_unique<HirearchyWindow>(this));
        m_windows.emplace_back(std::make_unique<ComponentWindow>(this));

        const aiko::MeshAsset defaultCube = aiko::mesh::factory::generateCube();

        auto root = Instantiate("Root");

        auto m_go1 = Instantiate(root, "Cube1");
        m_go1->transform().position = { 1.0f, 0.0f, 0.0f };
        m_go1->transform().rotation = { 0.0f, 0.0f, 0.0f };
        m_go1->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto mesh1 = m_go1->addComponent<MeshComponent>();
        mesh1->load(defaultCube);

        auto m_go2 = Instantiate(root, "Cube2");
        m_go2->transform().position = { -1.0f, 0.0f, 0.0f };
        m_go2->transform().rotation = { 0.0f, 0.0f, 0.0f };
        m_go2->transform().scale = { 1.0f, 1.0f, 1.0f };
        auto mesh2 = m_go2->addComponent<MeshComponent>();
        mesh2->load(defaultCube);

    }

    void AikoEditor::render()
    {
        // Docking Space// Docking Space
        auto main_viewport = ImGui::GetMainViewport();
        ImGui::DockSpaceOverViewport(main_viewport->ID);

        for (auto& tmp : m_windows)
        {
            if (tmp->isOpen())
            {
                tmp->render();
            }
        }
    }

    void AikoEditor::requestClose()
    {
        WindowCloseEvent event;
        EventSystem::it().sendEvent(event);
    }

}
