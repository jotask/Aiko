#pragma once

#include <math/math.h>

namespace aiko
{
    class Display
    {
    public:
        ivec2 getDisplaySize() const;
        void setWindowSize(int width, int height);                  // Set window dimensions
        std::string getWindowTitle() const;
        void setWindowTitle(string title);                     // Set title for window (only PLATFORM_DESKTOP and PLATFORM_WEB)
        void setWindowPosition(int x, int y);                       // Set window position on screen (only PLATFORM_DESKTOP)
    private:
        ivec2 m_size;
        string m_tittle;
    };

}