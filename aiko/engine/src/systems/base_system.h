#pragma once

#include "systems/sytem.h"

namespace aiko
{
    
    class BaseSystem : public System
    {
    public:
    
        BaseSystem() = default;
        virtual ~BaseSystem() = default;
    
    protected:
    
        virtual void connect(ModuleConnector*, SystemConnector*) { };
    
        virtual void init() override { };
    
        virtual void update() override { };
    
        virtual void render() override { };
    
        virtual void dispose() override { };
    
    };

}
