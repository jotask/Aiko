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

            enum class PendingAction
            {
                None,
                NewScene,
                OpenScene,
                Exit
            };

            void handleShortcuts(EditorContext& context);

            void newScene(EditorContext& context);
            void openSceneDialog();
            void requestAction(EditorContext& context, PendingAction action);
            void performAction(EditorContext& context, PendingAction action);
            void renderUnsavedChangesPopup(EditorContext& context);

            bool saveScene(EditorContext& context);
            void openSaveDialog(EditorContext& context);
            void duplicateSelected(EditorContext& context);

            EditorWorkspace* m_workspace = nullptr;
            RuntimeContext* m_runtime = nullptr;

            PendingAction m_pendingAction = PendingAction::None;
        };

    }
}
