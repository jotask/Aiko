#pragma once

#include "aiko_types.h"
#include "base_module.h"
#include "shared/math.h"

namespace aiko
{
    
    class DisplayModule : public BaseModule
    {
    
    public:
    
        DisplayModule() = default;
        virtual ~DisplayModule();



        bool isOpen();

        ivec2 getDisplaySize();

        void setWindowTitle(const char* title);                     // Set title for window (only PLATFORM_DESKTOP and PLATFORM_WEB)
        void setWindowPosition(int x, int y);                       // Set window position on screen (only PLATFORM_DESKTOP)
        void setWindowSize(int width, int height);                  // Set window dimensions

    protected:

        // TODO Get this size from config
        const int screenWidth = 800;
        const int screenHeight = 450;
    
        virtual void init() override;
        virtual void preUpdate() override;
    
    };

}
