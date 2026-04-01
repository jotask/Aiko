#pragma once

#include "window.h"

#include <aiko_includes.h>
#include "editor_menu_item/menu_item.h"

#include "serializer/scene_serializer_YAML.h"

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

            vector<AikoPtr<MenuItem>> m_items;

            bool m_isImguiExampleOpen = false;

        };

    }
}
