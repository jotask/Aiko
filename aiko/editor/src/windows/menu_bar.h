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
            virtual void init() override;
            virtual void render() override;
        private:
            std::vector<AikoPtr<MenuItem>> m_items;
        };

    }
}
