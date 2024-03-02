#pragma once

#include <random>
#include <sstream>

#include "shared/math.h"

namespace aiko
{

    class Display
    {
    public:
        ivec2 getDisplaySize();
        void setWindowTitle(const char* title);                     // Set title for window (only PLATFORM_DESKTOP and PLATFORM_WEB)
        void setWindowPosition(int x, int y);                       // Set window position on screen (only PLATFORM_DESKTOP)
        void setWindowSize(int width, int height);                  // Set window dimensions

    };

}