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
        UnloadRenderTexture(m_renderTexture2D);
    }
    
    void RenderComponentTexture::preInit()
    {

    }
    
    void RenderComponentTexture::init()
    {
    }
    
    void RenderComponentTexture::postInit()
    {
        m_pixels = getPixels();
        m_renderTexture2D = LoadRenderTexture(screenWidth, screenHeight);
    }
    
    void RenderComponentTexture::preUpdate()
    {
        deltaTime = GetFrameTime();
        timer += deltaTime;
    }
    
    void RenderComponentTexture::update()
    {

    }
    
    void RenderComponentTexture::postUpdate()
    {
    }
    
    void RenderComponentTexture::preRender()
    {
    }
    
    void RenderComponentTexture::render()
    {
        UpdateTexture(m_renderTexture2D.texture, m_pixels.data());
    }
    
    void RenderComponentTexture::postRender()
    {
        ClearBackground(WHITE);
        
        DrawTexturePro(
            m_renderTexture2D.texture,
            Rectangle{ 0, 0, static_cast<float>(m_renderTexture2D.texture.width), static_cast<float>(-m_renderTexture2D.texture.height) },
            Rectangle{ 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) },
            Vector2{ 0, 0 },
            0,
            WHITE
        );
    }
    
    void RenderComponentTexture::setPixels(std::vector<Color> pixels)
    {
        m_pixels = pixels;
    }

}
