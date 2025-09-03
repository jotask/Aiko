#ifdef AIKO_BGFX

#pragma once

#include "aiko_types.h"
#include "modules/display_module.h"
#include "shared/math.h"
#include "models/display.h"
#include "events/event.hpp"

namespace aiko::bgfx
{

    class BgfxDisplayModule : public aiko::DisplayModule
    {

    public:

        BgfxDisplayModule(Aiko* aiko) : aiko::DisplayModule(aiko) { };
        virtual ~BgfxDisplayModule();

    protected:

        virtual void init() override;
        virtual void preUpdate() override;

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual void dispose() override;

    };

}
#endif