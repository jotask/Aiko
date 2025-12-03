#pragma once

#include <aiko_types.h>

#include "display/display.h"

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

        Display m_display;
        GLFWwindow* m_native;

    };

}