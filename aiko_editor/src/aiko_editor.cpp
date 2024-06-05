#include "aiko_editor.h"

#include "windows/game_window.h"
#include "windows/hirearchy_window.h"
#include "windows/menu_bar.h"
#include "windows/component_window.h"

#include <aiko_includes.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace aiko::editor
{
    AikoEditor::AikoEditor()
        : aiko::Application(aiko::AikoConfig("Aiko Editor", 1024, 768, aiko::AikoConfig::DEFAULT_BACKGROUND_COLOR, false))
    {

    }

    aiko::Aiko* AikoEditor::getAiko() const
    {
        return m_aiko.get();
    }

    void AikoEditor::init()
    {
        // https://www.codingwiththomas.com/blog/rendering-an-opengl-framebuffer-into-a-dear-imgui-window
        Application::init();

        auto camera = this->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(camera::CameraController::Orbit);

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        m_windows.emplace_back(std::make_unique<MenuBar>(this));
        m_windows.emplace_back(std::make_unique<GameWindow>(this));
        m_windows.emplace_back(std::make_unique<HirearchyWindow>(this));
        m_windows.emplace_back(std::make_unique<ComponentWindow>(this));

        auto root = Instantiate("Root");

        auto m_go1 = this->Instantiate(root, "Cube1");
        m_go1->transform()->position = { 1.0f, 0.0f, 0.0f };
        m_go1->transform()->rotation = { 0.0f, 0.0f, 0.0f };
        m_go1->transform()->scale = { 1.0f, 1.0f, 1.0f };
        auto mesh1 = m_go1->addComponent<aiko::MeshComponent>();

        auto m_go2 = this->Instantiate(root, "Cube2");
        m_go2->transform()->position = { -1.0f, 0.0f, 0.0f };
        m_go2->transform()->rotation = { 0.0f, 0.0f, 0.0f };
        m_go2->transform()->scale = { 1.0f, 1.0f, 1.0f };
        auto mesh2 = m_go2->addComponent<aiko::MeshComponent>();

        auto m_texture = this->Instantiate(root, "Texture");
        m_texture->transform()->position = { 0.0f, -0.55f, 0.0f };
        m_texture->transform()->rotation = { 0.0f, 0.0f, 0.0f };
        m_texture->transform()->scale = { 1.0f, 1.0f, 1.0f };
        auto mesh3 = m_texture->addComponent<aiko::TextureComponent>();

        auto m_texturePbo = this->Instantiate(root, "PboTexture");
        m_texturePbo->transform()->position = { 0.0f, 0.55f, 0.0f };
        m_texturePbo->transform()->rotation = { 0.0f, 0.0f, 0.0f };
        m_texturePbo->transform()->scale = { 1.0f, 1.0f, 1.0f };
        auto mesh4 = m_texturePbo->addComponent<aiko::PboTextureComponent>();

    }

    void AikoEditor::render()
    {
        Application::render();
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
}
