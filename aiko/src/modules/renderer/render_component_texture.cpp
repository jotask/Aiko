#include "render_component_texture.h"

#include <vector>
#include <cmath>
#include <limits>

#include <raylib.h>

#include "config.h"
#include "modules/module_connector.h"
#include "modules/renderer/render_module.h"

namespace aiko
{
    
    RenderComponentTexture::RenderComponentTexture(RenderModule* renderModule)
        : RendererComponent(renderModule)
        , m_renderTexture2D()
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
        auto size = m_renderModule->getDisplaySize();
        m_renderTexture2D = LoadRenderTexture(size.x, size.y);
    }
    
    void RenderComponentTexture::postInit()
    {
    }
    
    void RenderComponentTexture::preUpdate()
    {
    }
    
    void RenderComponentTexture::update()
    {
        if (IsWindowResized() == true && IsWindowFullscreen() == false)
        {
            auto screenWidth = GetScreenWidth();
            auto screenHeight = GetScreenHeight();
            UnloadRenderTexture(m_renderTexture2D);
            m_renderTexture2D = LoadRenderTexture(screenWidth, screenHeight);
        }
    }
    
    void RenderComponentTexture::postUpdate()
    {
    }
    
    void RenderComponentTexture::preRender()
    {
        // We are done, render to the texture
        BeginTextureMode(m_renderTexture2D);
        ClearBackground(BLACK);
        DrawRectangle(0, 0, m_renderTexture2D.texture.width, m_renderTexture2D.texture.height, BLACK);
        EndTextureMode();
    }
    
    void RenderComponentTexture::render()
    {

    }
    
    void RenderComponentTexture::postRender()
    {
    }

}
