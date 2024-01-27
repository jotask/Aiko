#include "display_module.h"

#include <raylib.h>
#include <rlImGui.h>

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
    
    void DisplayModule::init()
    {
        // TODO Get this size from config
        const int screenWidth = 800;
        const int screenHeight = 450;
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(screenWidth, screenHeight, "Aiko");
        rlImGuiSetup(true);
    }
    
    

}
