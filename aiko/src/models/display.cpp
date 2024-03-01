#pragma once

#include <random>
#include <sstream>
#include "display.h"

#include "core/libs.h"

namespace aiko
{

    ivec2 Display::getDisplaySize()
    {
        return { GetScreenWidth(), GetScreenHeight() };
    }

    void Display::setWindowTitle(const char* title)
    {
        SetWindowTitle(title);
    }

    void Display::setWindowPosition(int x, int y)
    {
        SetWindowPosition(x, y);
    }

    void Display::setWindowSize(int width, int height)
    {
        SetWindowSize(width, height);
    }

}