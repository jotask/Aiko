#pragma once

#include <aiko_types.h>

#include "display/display.h"
#include "display/display_events.hpp"

class GLFWwindow;

namespace aiko
{

    class DisplayManager
    {

    public:

        DisplayManager() = default;
        ~DisplayManager() = default;

        void init(string title, const uint width, uint height);
        void update();
        void swap();
        void dispose();

        Display* getDisplay();
        const Display* getDisplay() const;
        GLFWwindow* getNativeWindow() const;

    private:

        Display m_display;
        GLFWwindow* m_native;

        bool centerWindow(GLFWwindow* window);
        void setupWindowCallbacks(GLFWwindow* window);

        void onKeyPressed(OnKeyPressedEvent& event);
        void onWindowResize(WindowResizeEvent& event);

    };

}