#include "display_module.h"

#include <core/libs.h>

#include "events/events.hpp"

namespace aiko
{
    
    DisplayModule::~DisplayModule()
    {
        CloseWindow();
    }
    
    void DisplayModule::init()
    {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        InitWindow(screenWidth, screenHeight, "Aiko");
        rlImGuiSetup(true);
    }

    void DisplayModule::preUpdate()
    {
        if (::IsWindowResized())
        {
            WindowResizeEvent even(::GetScreenWidth(), GetScreenHeight());
            aiko::EventSystem::it().sendEvent(even);
        }
        if (::WindowShouldClose())
        {
            WindowCloseEvent even;
            aiko::EventSystem::it().sendEvent(even);
        }
    }

}
