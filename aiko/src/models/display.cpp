#pragma once

#include <random>
#include <sstream>
#include "display.h"

#include "core/libs.h"

#ifdef AIKO_BUILD_RAYLIB
#include <raylib.h>
#endif

namespace aiko
{

    ivec2 Display::getDisplaySize()
    {
        return m_size;
    }

    void Display::setWindowTitle(const char* title)
    {
        // SetWindowTitle(title);
    }

    void Display::setWindowPosition(int x, int y)
    {
        // SetWindowPosition(x, y);
    }

    void Display::setWindowSize(int width, int height)
    {
        // SetWindowSize(width, height);
    }

}