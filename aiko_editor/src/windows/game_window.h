#pragma once

#include "window.h"

namespace aiko
{
    namespace editor
    {
        class AikoEditor;
        class GameWindow : public Window
        {
        public:
            GameWindow(AikoEditor*);
            ~GameWindow() = default;
            virtual void render();
        };

    }
}
