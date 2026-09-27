#include "main_menu_bar.h"

#include <imgui.h>

#include <display/display_events.hpp>
#include <events/events.hpp>

#include "ImGuiFileDialog.h"
#include "ImGuiFileDialogConfig.h"

#include "constants.h"
#include "core/editor_context.h"
#include "core/editor_workspace.h"
#include "serializer/scene_serializer_YAML.h"
#include "systems/scene_system.h"

namespace aiko::editor
{

    MainMenuBar::MainMenuBar()
        : EditorPanel("Main Menu")
    {
    }

    void MainMenuBar::setWorkspace(EditorWorkspace* workspace)
    {
        m_workspace = workspace;
    }

    void MainMenuBar::render(EditorContext& context)
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                {
                    IGFD::FileDialogConfig config;
                    config.path = global::GLOBAL_SCENE_FILES_PATH;
                    config.fileName = "editor.scene";

                    ImGuiFileDialog::Instance()->OpenDialog("loadFileDlgKey", "Choose File", ".scene", config);
                }

                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    IGFD::FileDialogConfig config;
                    config.path = global::GLOBAL_SCENE_FILES_PATH;
                    config.fileName = "editor.scene";

                    ImGuiFileDialog::Instance()->OpenDialog("saveChooseFileDlgKey", "Choose File", ".scene", config);
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit", "Alt+F4"))
                {
                    WindowCloseEvent event;
                    EventSystem::it().sendEvent(event);
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "Ctrl+Z", false, context.commands().canUndo()))
                {
                    context.clearSelection();
                    context.commands().undo();
                }

                if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z", false, context.commands().canRedo()))
                {
                    context.clearSelection();
                    context.commands().redo();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (m_workspace != nullptr)
                {
                    for (const auto& panel : m_workspace->panels())
                    {
                        if (panel == nullptr || panel.get() == this)
                        {
                            continue;
                        }

                        ImGui::MenuItem(panel->name().c_str(), nullptr, &panel->openState());
                    }
                }

                ImGui::Separator();

                EditorViewSettings& settings = context.viewSettings();

                ImGui::MenuItem("Light Gizmos", nullptr, &settings.showLightGizmos);

                ImGui::Separator();

                ImGui::MenuItem("Override Background", nullptr, &settings.overrideClearColor);

                if (settings.overrideClearColor)
                {
                    float color[4] =
                    {
                        settings.clearColor.r,
                        settings.clearColor.g,
                        settings.clearColor.b,
                        settings.clearColor.a
                    };

                    if (ImGui::ColorEdit4("Background Color", color))
                    {
                        settings.clearColor =
                        {
                            color[0],
                            color[1],
                            color[2],
                            color[3]
                        };
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (ImGuiFileDialog::Instance()->Display("saveChooseFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                const string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                const Scene& scene = context.sceneSystem().getScene();
                SceneSerializerYAML::serializeScene(scene, filePathName);
            }

            ImGuiFileDialog::Instance()->Close();
        }

        if (ImGuiFileDialog::Instance()->Display("loadFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                const string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

                context.clearSelection();
                context.commands().clear();

                Scene& scene = context.sceneSystem().getScene();
                SceneSerializerYAML::deserializeScene(scene, filePathName);
            }

            ImGuiFileDialog::Instance()->Close();
        }
    }

}