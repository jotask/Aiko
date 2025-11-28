#pragma once

#include <random>
#include <sstream>

#include "shared/math.h"

namespace aiko
{

    class Display
    {
        friend class DisplayModule;
    public:
        void* getNative();
        void setNative(void* n);
        ivec2 getDisplaySize();
        void setWindowTitle(const char* title);                     // Set title for window (only PLATFORM_DESKTOP and PLATFORM_WEB)
        std::string getWindowTitle() const;
        void setWindowPosition(int x, int y);                       // Set window position on screen (only PLATFORM_DESKTOP)
        void setWindowSize(int width, int height);                  // Set window dimensions
    private:
        void* native;
        ivec2 m_size;
        std::string tittle;
    };

}