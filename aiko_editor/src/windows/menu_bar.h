#pragma once

#include "window.h"

#include <aiko_includes.h>
#include "editor_menu_item/menu_item.h"

namespace aiko
{
    namespace editor
    {
        class AikoEditor;
        class MenuBar : public Window
        {
        public:
            MenuBar(AikoEditor*);
            ~MenuBar() = default;
            virtual void render();
        private:
            std::vector<aiko::AikoPtr<MenuItem>> m_items;
        };

    }
}
