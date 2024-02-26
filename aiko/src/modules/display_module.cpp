#include "display_module.h"

#include "core/libs.h"

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

    vec2 DisplayModule::getDisplaySize()
    {
        return { GetScreenWidth(), GetScreenHeight()};
    }
    
    void DisplayModule::init()
    {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(screenWidth, screenHeight, "Aiko");
        rlImGuiSetup(true);
    }
    
    

}
