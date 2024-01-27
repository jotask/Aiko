#pragma once

namespace aiko
{
    
    class ModuleConnector;
    class SystemConnector;
    
    class System
    {
    public:
    
        friend class Aiko;
    
    protected:
    
        Aiko* aiko;
    
        virtual void connect(ModuleConnector*c, SystemConnector*) = 0;
        
        virtual void init() = 0;
        
        virtual void update() = 0;
        
        virtual void render() = 0;
    
        virtual void dispose() = 0;
    
    private:
    
    };

    }
