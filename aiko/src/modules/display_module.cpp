#include "display_module.h"

#include <core/libs.h>

namespace aiko
{
    
    DisplayModule::~DisplayModule()
    {
        CloseWindow();
    }
    
    bool DisplayModule::isOpen()
    {
        return !WindowShouldClose();
    }

    ivec2 DisplayModule::getDisplaySize()
    {
        return { GetScreenWidth(), GetScreenHeight()};
    }

    void DisplayModule::setWindowTitle(const char* title)
    {
        SetWindowTitle(title);
    }

    void DisplayModule::setWindowPosition(int x, int y)
    {
        SetWindowPosition(x, y);
    }

    void DisplayModule::setWindowSize(int width, int height)
    {
        SetWindowSize(width, height);
    }
    
    void DisplayModule::init()
    {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(screenWidth, screenHeight, "Aiko");
        rlImGuiSetup(true);
    }
    
    

}
