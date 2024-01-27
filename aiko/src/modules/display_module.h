#pragma once

#include "aiko_types.h"
#include "base_module.h"

namespace aiko
{
    
    class DisplayModule : public BaseModule
    {
    
    public:
    
        DisplayModule() = default;
        virtual ~DisplayModule();
    
        bool isOpen();

        vec2 getDisplaySize();
    
    protected:

        // TODO Get this size from config
        const int screenWidth = 800;
        const int screenHeight = 450;
    
        virtual void init() override;
    
    };

}
