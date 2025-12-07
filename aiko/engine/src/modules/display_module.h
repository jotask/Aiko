#pragma once

#include <aiko_types.h>
#include <display/display.h>
#include <math/math.h>
#include <events/event.hpp>

#include "base_module.h"

namespace aiko
{
    
    class DisplayModule : public BaseModule
    {
    
    public:

        DisplayModule(Aiko* aiko) : BaseModule(aiko) { };
        virtual ~DisplayModule() = default;

        Display& getCurrentDisplay() { return m_curent; };

    protected:

        virtual void preInit() override;

        Display m_curent;

        void onWindowResize(Event&);
    
    };

}
