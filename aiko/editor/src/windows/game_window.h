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
            virtual ~GameWindow() = default;
            virtual void init() override;
            virtual void render() override;
        };

    }
}
