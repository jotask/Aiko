#pragma once

#include "core/editor_panel.h"

namespace aiko
{

    class RuntimeContext;

    namespace editor
    {
        class EditorWorkspace;

        class MainMenuBar final : public EditorPanel
        {
        public:
            MainMenuBar();

            void render(EditorContext& context) override;

            void setWorkspace(EditorWorkspace* workspace);
            void setRuntime(RuntimeContext* runtime);

        private:

            enum class SaveResult
            {
                Saved,
                AwaitingPath,
                Failed
            };

            enum class PendingAction
            {
                None,
                NewScene,
                OpenScene,
                Exit
            };

            void handleShortcuts(EditorContext& context);

            void newScene(EditorContext& context);
            void openSceneDialog(EditorContext& context);
            void requestAction(EditorContext& context, PendingAction action);
            void performAction(EditorContext& context, PendingAction action);
            void renderUnsavedChangesPopup(EditorContext& context);
            void renderSimulationControls();


            SaveResult saveScene(EditorContext& context);
            bool saveSceneToPath(EditorContext& context, const string& path);
            void openSaveDialog(EditorContext& context);
            void duplicateSelected(EditorContext& context);

            void renderError(const char* tittle, string& error);

            EditorWorkspace* m_workspace = nullptr;
            RuntimeContext* m_runtime = nullptr;

            string m_saveError;
            string m_loadError;

            PendingAction m_pendingAction = PendingAction::None;
        };

    }
}
