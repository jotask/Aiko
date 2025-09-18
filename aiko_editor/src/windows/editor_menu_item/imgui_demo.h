#pragma once

#include "windows/editor_menu_item/menu_item.h"

namespace aiko
{
    namespace editor
    {
        class AikoEditor;
        class ImGuiDemo : public MenuItem
        {
        public:
            ImGuiDemo(AikoEditor*);
            ~ImGuiDemo() = default;
            virtual void render() override;
        };

    }
}
