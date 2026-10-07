#pragma once

#include "core/editor_panel.h"

namespace aiko::editor
{

    class GameViewPanel final : public EditorPanel
    {
    public:
        GameViewPanel();

        void render(EditorContext& context) override;
    };

}