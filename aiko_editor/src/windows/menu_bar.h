#pragma once

#include "window.h"

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
        };

    }
}
