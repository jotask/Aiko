#include "display.h"

namespace aiko
{

    ivec2 Display::getDisplaySize() const
    {
        return m_size;
    }

    void Display::setWindowTitle(string title)
    {
        m_tittle = title;
    }

    std::string Display::getWindowTitle() const
    {
        return m_tittle;
    }

    void Display::setWindowPosition(int x, int y)
    {

    }

    void Display::setWindowSize(int width, int height)
    {
        m_size = {width, height};
    }

}