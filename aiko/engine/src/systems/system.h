#pragma once

namespace aiko
{

    class Aiko;
    class ModuleConnector;
    class SystemConnector;
    
    class System
    {
        friend class Aiko;
    protected:

        virtual void connect(ModuleConnector*c, SystemConnector*) = 0;
        
        virtual void init() = 0;
        
        virtual void update() = 0;

        virtual void render() = 0;
    
        virtual void dispose() = 0;
    
    private:
    
    };

    }
