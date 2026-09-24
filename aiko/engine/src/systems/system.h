#pragma once

#include "systems/system_phase.h"

namespace aiko
{

    class Aiko;
    class ModuleConnector;
    class SystemConnector;
    
    class System
    {
        friend class Aiko;
    public:
        virtual ~System() = default;

    protected:

        virtual SystemUpdatePhase updatePhase() const { return SystemUpdatePhase::Gameplay; }
        virtual SystemRenderPhase renderPhase() const { return SystemRenderPhase::RenderPreparation; }

        virtual void connect(ModuleConnector*c, SystemConnector*) = 0;
        
        virtual void init() = 0;
        
        virtual void update() = 0;

        virtual void render() = 0;
    
        virtual void dispose() = 0;
    
    private:
    
    };

    }
