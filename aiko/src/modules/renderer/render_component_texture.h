#pragma once

#include <vector>

#include "raylib.h"

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
    
        virtual aiko::vec2 getDisplayViewport() override { return { screenWidth, screenHeight }; };
    
        std::vector<Color> getPixels() { return m_pixels; };
        void setPixels(std::vector<Color> pixels);
    
    private:
    

        const int resolution = 1;

        const int screenWidth = 100;
        const int screenHeight = 100;
    
        RenderTexture2D m_renderTexture2D;
        std::vector<Color> m_pixels;
    
        // temporal
        float timer = 0.0f;
        float deltaTime = 0.0f;
    
    
    };

}
