#pragma once

#include "core/editor_panel.h"

namespace aiko
{
    namespace editor
    {
        class InspectorPanel final : public EditorPanel
        {
        public:
            InspectorPanel();
            virtual void render(EditorContext& context) override;
        };

    }
}
