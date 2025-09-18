#pragma once

#include "window.h"

namespace aiko
{
    namespace editor
    {
        class AikoEditor;
        class HirearchyWindow : public Window
        {
        public:
            HirearchyWindow(AikoEditor*);
            ~HirearchyWindow() = default;
            virtual void render();
        };

    }
}
