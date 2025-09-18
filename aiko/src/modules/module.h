#pragma once

namespace aiko
{
    
    class ModuleConnector;
    
    class Module
    {
    public:
    
        friend class Aiko;
    
    protected:

        Module(Aiko* aiko) : aiko(aiko) { };
    
        virtual void connect(ModuleConnector*) = 0;
        
        virtual void preInit() = 0;
        virtual void init() = 0;
        virtual void postInit() = 0;
        
        virtual void preUpdate() = 0;
        virtual void update() = 0;
        virtual void postUpdate() = 0;
        
        virtual void preRender() = 0;
        virtual void render() = 0;
        virtual void postRender() = 0;
        
        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;
    
        virtual void dispose() = 0;
    
        Aiko const* getAiko() const { return aiko; }

    private:
        Aiko const* aiko;
    };

    }
