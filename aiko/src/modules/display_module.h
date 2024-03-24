#pragma once

#include "aiko_types.h"
#include "base_module.h"
#include "shared/math.h"
#include "models/display.h"

namespace aiko
{
    
    class DisplayModule : public BaseModule
    {
    
    public:
    
        DisplayModule() = default;
        virtual ~DisplayModule();

        Display& getCurrentDisplay() { return m_curent; };

        void* getNativeDisplay();

    protected:

        // TODO Get this size from config
        const int screenWidth = 800;
        const int screenHeight = 450;

        virtual void init() override;
        virtual void preUpdate() override;
        virtual void endFrame() override;

        Display m_curent;
    
    };

}
