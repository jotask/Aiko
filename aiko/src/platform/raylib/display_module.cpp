#ifdef AIKO_BUILD_RAYLIB
#include "modules/display_module.h"

#include <core/libs.h>

#include "events/events.hpp"

#ifdef AIKO_BUILD_RAYLIB
#include <raylib.h>
#include <rlImGui.h>
#endif

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
#endif
