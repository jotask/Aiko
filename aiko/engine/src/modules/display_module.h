#pragma once

#include "aiko.h"
#include "aiko_types.h"
#include "base_module.h"
#include "shared/math.h"
#include "models/display.h"
#include "events/event.hpp"

namespace aiko
{
    
    class DisplayModule : public BaseModule
    {
    
    public:

        DisplayModule(Aiko* aiko) : BaseModule(aiko) { };
        virtual ~DisplayModule() = default;

        Display& getCurrentDisplay() { return m_curent; };

        void* getNativeDisplay();

    protected:

        virtual void preInit() override;

        Display m_curent;

        string m_displayName;

        void onWindowResize(Event&);
    
    };

}
