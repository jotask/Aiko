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
            EditorWorkspace* m_workspace = nullptr;
        };

    }
}
