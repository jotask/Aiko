#include "render_component_pixel.h"

#include <vector>
#include <cmath>
#include <limits>

#include "core/libs.h"

#include "config.h"
#include "modules/module_connector.h"
#include "modules/renderer/render_module.h"

namespace aiko
{
    
    RenderComponentPixel::RenderComponentPixel(RenderModule* renderModule)
        : RendererComponent(renderModule)
        , m_renderTexture2D()
        , m_pixels( std::vector<Color>(screenWidth * screenHeight * sizeof(Color)) )
    {
    }
    
    RenderComponentPixel::~RenderComponentPixel()
    {
        UnloadRenderTexture(m_renderTexture2D);
    }
    
    void RenderComponentPixel::preInit()
    {

    }
    
    void RenderComponentPixel::init()
    {
        auto size = m_renderModule->getDisplaySize();
        m_renderTexture2D = LoadRenderTexture(size.x, size.y);
    }
    
    void RenderComponentPixel::postInit()
    {
        m_pixels = getPixels();
        m_renderTexture2D = LoadRenderTexture(screenWidth, screenHeight);
    }
    
    void RenderComponentPixel::preUpdate()
    {
    }
    
    void RenderComponentPixel::update()
    {
        if (IsWindowResized() == true && IsWindowFullscreen() == false)
        {
            auto screenWidth = GetScreenWidth();
            auto screenHeight = GetScreenHeight();
            UnloadRenderTexture(m_renderTexture2D);
            m_renderTexture2D = LoadRenderTexture(screenWidth, screenHeight);
        }
        for (auto& p : m_pixels)
        {
            p = ColorFromHSV(GetRandomValue(0.0f, 2560.f),1.0f, 0.5);
        }
        setPixels(m_pixels);
    }
    
    void RenderComponentPixel::postUpdate()
    {
    }
    
    void RenderComponentPixel::preRender()
    {
        // We are done, render to the texture
        BeginTextureMode(m_renderTexture2D);
        ClearBackground(BLACK);
        DrawRectangle(0, 0, m_renderTexture2D.texture.width, m_renderTexture2D.texture.height, BLACK);
        EndTextureMode();
    }
    
    void RenderComponentPixel::render()
    {
        UpdateTexture(m_renderTexture2D.texture, m_pixels.data());
    }
    
    void RenderComponentPixel::postRender()
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
    
    void RenderComponentPixel::setPixels(std::vector<Color> pixels)
    {
        m_pixels = pixels;
    }

}
