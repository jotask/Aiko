#pragma once

#include <map>

#include "base_module.h"
#include "types/inputs.h"
#include "shared/math.h"
#include "events/event.hpp"

namespace aiko
{

    class DisplayModule;
    class DebugModule : public BaseModule
    {
    public:
    
        DebugModule(Aiko* aiko) : BaseModule(aiko) { };
        virtual ~DebugModule() = default;

    protected:

        virtual void connect(ModuleConnector*);

        virtual void init() override;

        virtual void preRender() override;
        virtual void render() override;
        virtual void postRender() override;

    private:
        DisplayModule* m_displayModule;
        void onKeyPressed(Event& event);

    };

}
