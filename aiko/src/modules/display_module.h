#pragma once

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
        virtual ~DisplayModule();

        Display& getCurrentDisplay() { return m_curent; };

        void* getNativeDisplay();

    protected:

        virtual void init() override;
        virtual void preUpdate() override;

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual void dispose() override;

        Display m_curent;

        std::string m_displayName;

        void onWindowResize(Event&);
    
    };

}
