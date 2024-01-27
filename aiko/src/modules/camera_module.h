#pragma once

#include "base_module.h"

namespace aiko
{
    
    class CameraModule : public BaseModule
    {
    public:
    
        CameraModule() = default;
        virtual ~CameraModule() = default;
    
    protected:
    
        virtual void init() override;
    
    };

}
