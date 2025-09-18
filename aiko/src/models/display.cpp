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

    void* Display::getNative()
    {
        return native;
    }

    void Display::setNative(void* n)
    {
        native = n;
    }

    ivec2 Display::getDisplaySize()
    {
        return m_size;
    }

    void Display::setWindowTitle(const char* title)
    {
        tittle = title;
    }

    std::string Display::getWindowTitle() const
    {
        return tittle;
    }

    void Display::setWindowPosition(int x, int y)
    {

    }

    void Display::setWindowSize(int width, int height)
    {
        m_size = {width, height};
    }

}