#include "main_menu_bar.h"

#include "ImGuiFileDialog.h"
#include "ImGuiFileDialogConfig.h"
#include "constants.h"
#include "core/editor_context.h"
#include "core/editor_workspace.h"
#include "serializer/scene_serializer_YAML.h"
#include "systems/scene_system.h"
#include "commands/game_object/duplicate_game_object_command.h"
#include <layers/contexts/runtime_context.h>

#include <display/display_events.hpp>
#include <events/events.hpp>

#include <filesystem>
#include <imgui.h>

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

    void MainMenuBar::setRuntime(RuntimeContext* runtime)
    {
        m_runtime = runtime;
    }

    void MainMenuBar::newScene(EditorContext& context)
    {
        context.clearSelection();
        context.commands().clear();
        Scene& scene = context.sceneSystem().getScene();
        scene.clear();
        context.document().reset();
    }

    void MainMenuBar::openSceneDialog()
    {
        IGFD::FileDialogConfig config;
        config.path = global::GLOBAL_SCENE_FILES_PATH;
        config.fileName = "editor.scene";
        ImGuiFileDialog::Instance()->OpenDialog("loadFileDlgKey", "Choose File", ".scene", config);
    }

    void MainMenuBar::requestAction(EditorContext& context, PendingAction action)
    {
        if (context.document().isDirty() == false)
        {
            performAction(context, action);
            return;
        }
        m_pendingAction = action;
        ImGui::OpenPopup("Unsaved Changes");
    }

    void MainMenuBar::performAction(EditorContext& context, PendingAction action)
    {
        switch (action)
        {
            case PendingAction::None:
                break;

            case PendingAction::NewScene:
                newScene(context);
                break;

            case PendingAction::OpenScene:
                openSceneDialog();
                break;

            case PendingAction::Exit:
            {
                WindowCloseEvent event;
                EventSystem::it().sendEvent(event);
                break;
            }
        }
    }

    void MainMenuBar::renderUnsavedChangesPopup(EditorContext& context)
    {
        if (ImGui::BeginPopupModal("Unsaved Changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextUnformatted("The current scene has unsaved changes.");
            ImGui::TextUnformatted("Do you want to save them before continuing?");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Save"))
            {
                if (saveScene(context))
                {
                    const PendingAction action = m_pendingAction;
                    m_pendingAction = PendingAction::None;

                    ImGui::CloseCurrentPopup();

                    performAction(context, action);
                }
                else
                {
                    // Save As dialog is now open.
                    // Keep m_pendingAction so it can continue after saving.
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Discard"))
            {
                const PendingAction action = m_pendingAction;
                m_pendingAction = PendingAction::None;

                ImGui::CloseCurrentPopup();

                performAction(context, action);
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel"))
            {
                m_pendingAction = PendingAction::None;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    bool MainMenuBar::saveScene(EditorContext& context)
    {
        if (context.document().hasPath())
        {
            const Scene& scene = context.sceneSystem().getScene();
            SceneSerializerYAML::serializeScene(scene, context.document().path());
            context.document().markSaved();
            return true;
        }
        openSaveDialog(context);
        return false;
    }

    void MainMenuBar::openSaveDialog(EditorContext& context)
    {
        IGFD::FileDialogConfig config;
        config.path = global::GLOBAL_SCENE_FILES_PATH;
        config.fileName = context.document().hasPath() ? std::filesystem::path(context.document().path()).filename().string() : "untitled.scene";
        ImGuiFileDialog::Instance()->OpenDialog("saveSceneDlg", "Save Scene", ".scene", config);
    }

    void MainMenuBar::duplicateSelected(EditorContext& context)
    {
        GameObject* selected = context.selectedGameObject();
        if (selected == nullptr)
        {
            return;
        }
        DuplicateGameObjectCommand& command = context.commands().execute<DuplicateGameObjectCommand>(context.sceneSystem(), *selected);
        context.select(command.duplicatedObject());
    }

    void MainMenuBar::handleShortcuts(EditorContext& context)
    {
        const ImGuiIO& io = ImGui::GetIO();

        if (io.WantTextInput)
        {
            return;
        }

        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_N, false))
        {
            requestAction(context, PendingAction::NewScene);
        }

        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_O, false))
        {
            requestAction(context, PendingAction::OpenScene);
        }

        if (io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_S, false))
        {
            openSaveDialog(context);
        }
        else if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false))
        {
            saveScene(context);
        }

        if (io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z, false))
        {
            if (context.commands().canRedo())
            {
                context.clearSelection();
                context.commands().redo();
            }
        }
        else if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z, false))
        {
            if (context.commands().canUndo())
            {
                context.clearSelection();
                context.commands().undo();
            }
        }

        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D, false))
        {
            duplicateSelected(context);
        }

        if (m_runtime != nullptr)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_F6, false))
            {
                m_runtime->setPaused(!m_runtime->isPaused());
            }

            if (m_runtime->isPaused() &&
                ImGui::IsKeyPressed(ImGuiKey_F7, false))
            {
                m_runtime->step();
            }
        }

    }

    void MainMenuBar::render(EditorContext& context)
    {
        handleShortcuts(context);
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {

                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                {
                    requestAction(context, PendingAction::NewScene);
                }

                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                {
                    requestAction(context, PendingAction::OpenScene);
                }

                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    saveScene(context);
                }

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                {
                    openSaveDialog(context);
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit", "Alt+F4"))
                {
                    requestAction(context, PendingAction::Exit);
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

                const bool canDuplicate = context.selectedGameObject() != nullptr;

                if (ImGui::MenuItem("Duplicate", "Ctrl+D", false, canDuplicate))
                {
                    duplicateSelected(context);
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Simulation"))
            {
                AIKO_ASSERT(m_runtime != nullptr, "MainMenuBar has no RuntimeContext");

                if (m_runtime != nullptr)
                {
                    const bool paused = m_runtime->isPaused();

                    if (paused == false)
                    {
                        if (ImGui::MenuItem("Pause", "F6"))
                        {
                            m_runtime->setPaused(true);
                        }
                    }
                    else
                    {
                        if (ImGui::MenuItem("Resume", "F6"))
                        {
                            m_runtime->setPaused(false);
                        }
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Step", "F7", false, paused))
                    {
                        m_runtime->step();
                    }
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
                ImGui::MenuItem("Camera Gizmos", nullptr, &settings.showCameraGizmos);
                ImGui::MenuItem("Show Grid", nullptr, &settings.showGrid);
                ImGui::MenuItem("Show Bounds", nullptr, &settings.showBounds);

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

        renderUnsavedChangesPopup(context);

        if (ImGuiFileDialog::Instance()->Display("saveSceneDlg"))
        {
            PendingAction action = PendingAction::None;
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                const string path = ImGuiFileDialog::Instance()->GetFilePathName();
                const Scene& scene = context.sceneSystem().getScene();
                SceneSerializerYAML::serializeScene(scene, path);
                context.document().setPath(path);
                context.document().markSaved();
                action = m_pendingAction;
            }
            m_pendingAction = PendingAction::None;
            ImGuiFileDialog::Instance()->Close();
            if (action != PendingAction::None)
            {
                performAction(context, action);
            }
        }

        if (ImGuiFileDialog::Instance()->Display("loadFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                const string path = ImGuiFileDialog::Instance()->GetFilePathName();

                context.clearSelection();
                context.commands().clear();

                Scene& scene = context.sceneSystem().getScene();

                SceneSerializerYAML::deserializeScene(scene, path);

                context.document().setPath(path);
                context.document().markSaved();
            }

            ImGuiFileDialog::Instance()->Close();
        }
    }

}
