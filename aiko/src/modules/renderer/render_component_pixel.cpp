#include "render_component_pixel.h"

#include <vector>
#include <cmath>
#include <limits>

#include <core/libs.h>

#include "config.h"
#include "modules/module_connector.h"
#include "modules/renderer/render_module.h"
#include "core/raylib_utils.h"
#include "core/textures.h"

#include <core/libs.h>

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
        auto texture = raylib::utils::toRaylibRenderTexture2D( m_renderTexture2D );
        UnloadRenderTexture(texture);
    }
    
    void RenderComponentPixel::preInit()
    {

    }
    
    void RenderComponentPixel::init()
    {
        auto size = m_renderModule->getDisplaySize();
        auto texture = LoadRenderTexture(size.x, size.y);
        m_renderTexture2D = raylib::utils::toRenderTexture2D(texture);
    }
    
    void RenderComponentPixel::postInit()
    {
        m_pixels = getPixels();
        auto texture = LoadRenderTexture(screenWidth, screenHeight);
        m_renderTexture2D = raylib::utils::toRenderTexture2D(texture);
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
            {
                auto texture = raylib::utils::toRaylibRenderTexture2D(m_renderTexture2D);
                UnloadRenderTexture(texture);
            }
            {
                auto texture = LoadRenderTexture(screenWidth, screenHeight);
                m_renderTexture2D = raylib::utils::toRenderTexture2D(texture);
            }
        }
        for (auto& p : m_pixels)
        {
            ::Color color = ColorFromHSV(GetRandomValue(0.0f, 2560.f),1.0f, 0.5);
            p = raylib::utils::toColor(color);
        }
        setPixels(m_pixels);
    }
    
    void RenderComponentPixel::postUpdate()
    {
    }
    
    void RenderComponentPixel::preRender()
    {
        // We are done, render to the texture
        auto texture = raylib::utils::toRaylibRenderTexture2D(m_renderTexture2D);
        BeginTextureMode(texture);
        ClearBackground(::BLACK);
        DrawRectangle(0, 0, m_renderTexture2D.texture.width, m_renderTexture2D.texture.height, ::BLACK);
        EndTextureMode();
    }
    
    void RenderComponentPixel::render()
    {
        auto texture = raylib::utils::toRaylibRenderTexture2D(m_renderTexture2D);
        UpdateTexture(texture.texture, m_pixels.data());
    }
    
    void RenderComponentPixel::postRender()
    {
        ClearBackground(::WHITE);
        auto texture = raylib::utils::toRaylibRenderTexture2D(m_renderTexture2D);
        DrawTexturePro(
            texture.texture,
            Rectangle{ 0, 0, static_cast<float>(m_renderTexture2D.texture.width), static_cast<float>(-m_renderTexture2D.texture.height) },
            Rectangle{ 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) },
            Vector2{ 0, 0 },
            0,
            ::WHITE
        );
    }
    
    void RenderComponentPixel::setPixels(std::vector<Color> pixels)
    {
        m_pixels = pixels;
    }

}
