#pragma once

#include "window.h"

#include <aiko_includes.h>

namespace aiko
{
    namespace editor
    {
        class AikoEditor;
        class ComponentWindow : public Window
        {
        public:
            ComponentWindow(AikoEditor*);
            ~ComponentWindow() = default;
            virtual void render();

        private:
            void onGameObjectSelected(aiko::Event& envt);
            aiko::GameObject* selectedGo;
        };

    }
}
