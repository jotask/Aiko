#pragma once

#include <vector>

#include "aiko_types.h"
#include "core/textures.h"
#include "core/render_types.h"
#include "modules/base_module.h"
#include "modules/scene_module.h"
#include "modules/renderer/renderer_component.h"
#include "core/color.h"

namespace aiko
{
    
    class RenderModule;
    class RenderComponentPixel : public RendererComponent
    {
    
    public:
    
        RenderComponentPixel(RenderModule*);
        virtual ~RenderComponentPixel();
    
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
    
        texture::RenderTexture2D& GetRendererTexture() { return m_renderTexture2D; };

        std::vector<Color> getPixels() { return m_pixels; };
        void setPixels(std::vector<Color> pixels);
    
    private:
    
        const int resolution = 1;

        const int screenWidth = 100;
        const int screenHeight = 100;
    
        texture::RenderTexture2D m_renderTexture2D;
        std::vector<Color> m_pixels;
    
    };

}
