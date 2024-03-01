#pragma once

#include <vector>

#include "aiko_types.h"
#include "modules/base_module.h"
#include "modules/scene_module.h"
#include "modules/renderer/renderer_component.h"

namespace aiko
{
    
    class RenderModule;
    class RenderComponentTexture : public RendererComponent
    {
    
    public:
    
        RenderComponentTexture(RenderModule*);
        virtual ~RenderComponentTexture();
    
        virtual void preInit() override;
        virtual void init() override;
        virtual void postInit() override;
    
        void virtual preUpdate() override;
        void virtual update() override;
        void virtual postUpdate() override;
    
        void virtual preRender() override;
        void virtual render() override;
        void virtual postRender() override;
    
        virtual aiko::ivec2 getDisplayViewport() override { return { screenWidth, screenHeight }; };
    
    private:
    

        const int resolution = 1;

        const int screenWidth = 100;
        const int screenHeight = 100;
    
    };

}
