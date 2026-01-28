#pragma once

#include "base_module.h"

namespace aiko
{
    
    class DisplayModule : public BaseModule
    {
    
    public:

        DisplayModule(Aiko* aiko) : BaseModule(aiko) { };
        virtual ~DisplayModule() = default
        ;
        virtual void preInit() override;
        virtual void preUpdate() override;
        virtual void endFrame() override;

    };

}
