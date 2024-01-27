#include "render_component_texture.h"

#include <vector>
#include <cmath>
#include <limits>

#include <raylib.h>

#include "config.h"
#include "modules/module_connector.h"

namespace aiko
{
    
    RenderComponentTexture::RenderComponentTexture(RenderModule* renderModule)
        : RendererComponent(renderModule)
        , m_renderTexture2D()
        , m_pixels( std::vector<Color>(screenWidth * screenHeight * sizeof(Color)) )
    {
    }
    
    RenderComponentTexture::~RenderComponentTexture()
    {
        UnloadTexture(m_renderTexture2D);
    }
    
    void RenderComponentTexture::preInit()
    {
    }
    
    void RenderComponentTexture::init()
    {
    }
    
    void RenderComponentTexture::postInit()
    {
    }
    
    void RenderComponentTexture::preUpdate()
    {
        deltaTime = GetFrameTime();
        timer += deltaTime;
    }
    
    void RenderComponentTexture::update()
    {
        m_renderTexture2D.width = screenWidth;
        m_renderTexture2D.height = screenHeight;
        m_renderTexture2D.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        m_renderTexture2D.mipmaps = 1;
    
    }
    
    void RenderComponentTexture::postUpdate()
    {
    }
    
    void RenderComponentTexture::preRender()
    {
    }
    
    void RenderComponentTexture::render()
    {
    }
    
    void RenderComponentTexture::postRender()
    {
        ClearBackground(WHITE);
        DrawTexturePro(
            m_renderTexture2D,
            Rectangle{ 0, 0, static_cast<float>(m_renderTexture2D.width), static_cast<float>(-m_renderTexture2D.height) },
            Rectangle{ 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) },
            Vector2{ 0, 0 },
            0,
            WHITE
        );
    }
    
    void RenderComponentTexture::setPixels(std::vector<Color> pixels)
    {
        m_pixels = pixels;
        UpdateTexture(m_renderTexture2D, m_pixels.data());
    }

}
