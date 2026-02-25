#pragma once

#include <aiko_types.h>
#include <core/singleton.h>

#include "display/display.h"
#include "display/display_events.hpp"

class GLFWwindow;

namespace aiko
{

    class DisplayManager : public Singleton<DisplayManager>
    {

    public:

        DisplayManager() = default;
        ~DisplayManager() = default;

        void init(string title, const uint width, uint height);
        void update();
        void swap();
        void dispose();

        Display* getDisplay();
        GLFWwindow* getNativeWindow() const;

    private:

        Display m_display;
        GLFWwindow* m_native;

        void onKeyPressed(Event& event);
        void onWindowResize(Event& event);

    };

}