#include "menu_bar.h"

#include "aiko_editor.h"
#include "aiko.h"
#include "systems/render_system.h"

#include "serializer/scene_serializer_YAML.h"
#include "ImGuiFileDialogConfig.h"
#include "ImGuiFileDialog.h"

#include "window.h"

#include "editor_menu_item/asset_manager.h"
#include "editor_menu_item/imgui_demo.h"

#include <imgui.h>

#include "systems/scene_system.h"

namespace aiko
{
    namespace editor
    {

        MenuBar::MenuBar(AikoEditor* editor)
            : Window(editor, "MenuBar")
        {
            m_items.emplace_back(std::make_unique<AssetManager>(editor));
            m_items.emplace_back(std::make_unique<ImGuiDemo>(editor));
        }

        void MenuBar::init()
        {
        }

        void MenuBar::render()
        {
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open...", "Ctrl+O"))
                    {
                        // Trigger file open dialog or handle opening files
                        IGFD::FileDialogConfig config;
                        config.path = ".";
                        ImGuiFileDialog::Instance()->OpenDialog("loadFileDlgKey", "Choose File", ".cpp,.h,.hpp", config);
                    }
                    if (ImGui::MenuItem("Save", "Ctrl+S"))
                    {
                        // Handle saving current work
                        IGFD::FileDialogConfig config;
                        config.path = ".";
                        ImGuiFileDialog::Instance()->OpenDialog("saveChooseFileDlgKey", "Choose File", ".cpp,.h,.hpp", config);
                    }
                    if (ImGui::MenuItem("Open ImGui Example", nullptr, &m_isImguiExampleOpen))
                    {

                    }
                    if (ImGui::MenuItem("Exit", "Alt+F4"))
                    {
                        // Handle exiting the application
                        getAiko()->close();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Editor"))
                {
                    for (auto& tmp : getEditor()->getWindows())
                    {
                        if (tmp.get() == this)
                        {
                            continue;
                        }
                        if (ImGui::MenuItem(tmp->getName(), nullptr, &tmp->is_open))
                        {
                            // Handle undo
                        }
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Extensions"))
                {
                    for (auto& tmp : m_items)
                    {
                        if (ImGui::MenuItem(tmp->getName(), nullptr, &tmp->is_open))
                        {
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
            for (auto& tmp : m_items)
            {
                if (tmp->is_open)
                {
                    tmp->render();
                }
            }
            if (m_isImguiExampleOpen == true)
            {
                ImGui::ShowDemoWindow(&m_isImguiExampleOpen);
            }

            if (ImGuiFileDialog::Instance()->Display("saveChooseFileDlgKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                { // action if OK
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                    // action
                    logger::Log::info("%s == %s", filePathName.c_str(), filePath.c_str());
                    SceneSystem* ecs = getAiko()->getSystem<SceneSystem>();
                    const Scene& scene = ecs->getScene();
                    SceneSerializerYAML::serializeScene(scene, filePathName);
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }

            if (ImGuiFileDialog::Instance()->Display("loadFileDlgKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                { // action if OK
                    std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                    // action
                    logger::Log::info("%s == %s", filePathName.c_str(), filePath.c_str());
                    SceneSystem* ecs = getAiko()->getSystem<SceneSystem>();
                    Scene& scene = ecs->getScene();
                    SceneSerializerYAML::deserializeScene(scene, filePathName);
                }

                // close
                ImGuiFileDialog::Instance()->Close();
            }

        }

    }
}
