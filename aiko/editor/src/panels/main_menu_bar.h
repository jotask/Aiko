#pragma once

#include "core/editor_panel.h"

namespace aiko
{
    namespace editor
    {
        class EditorWorkspace;

        class MainMenuBar final : public EditorPanel
        {
        public:
            MainMenuBar();

            void render(EditorContext& context) override;

            void setWorkspace(EditorWorkspace* workspace);

        private:
            void handleShortcuts(EditorContext& context);

            void newScene(EditorContext& context);
            void openSceneDialog();
            void saveScene(EditorContext& context);
            void openSaveDialog(EditorContext& context);

            EditorWorkspace* m_workspace = nullptr;
        };

    }
}
