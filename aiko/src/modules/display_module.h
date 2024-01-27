#pragma once

#include "base_module.h"

namespace aiko
{
    
    class DisplayModule : public BaseModule
    {
    
    public:
    
        DisplayModule() = default;
        virtual ~DisplayModule();
    
        bool isOpen();
    
    protected:
    
        virtual void init() override;
    
    };

}
