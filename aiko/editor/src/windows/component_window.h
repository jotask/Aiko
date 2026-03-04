#pragma once

#include "window.h"

#include <aiko_includes.h>

#include "events/editor_events.h"

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
            void onGameObjectSelected(HirearchyGameObjectSelectedEvent& envt);
            GameObject* selectedGo;
        };

    }
}
