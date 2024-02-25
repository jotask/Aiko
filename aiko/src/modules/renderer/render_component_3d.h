#pragma once

#include "aiko_types.h"
#include "modules/base_module.h"
#include "modules/scene_module.h"
#include "modules/renderer/renderer_component.h"

namespace aiko
{

    class CameraModule;
    class RenderModule;
    class RenderComponent3D : public RendererComponent
    {
    
    public:
    
        RenderComponent3D(RenderModule* renderModule);
        virtual ~RenderComponent3D() = default;
    
        virtual void preInit() override;
        virtual void init() override;
        virtual void postInit() override;
    
        void virtual preUpdate() override;
        void virtual update() override;
        void virtual postUpdate() override;
    
        void virtual preRender() override;
        void virtual render() override;
        void virtual postRender() override;
    
    protected:
    
        const int numBlocks = 15;
    
    };

}
