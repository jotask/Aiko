#ifdef AIKO_BGFX

#include "bgfx_render_module.h"

#include <fstream>

#include <iostream>
#include <set>
#include <vector>

#include <algorithm>

#include "aiko.h"
#include "constants.h"
#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"
#include "core/libs.h"
#include "core/log.h"

namespace aiko::bgfx
{

    BgfxRenderModule::BgfxRenderModule(Aiko* aiko)
        : RenderModule(aiko)
    {
    
    }

    BgfxRenderModule::~BgfxRenderModule()
    {
        
    }


    void BgfxRenderModule::preInit()
    {
    }



    void BgfxRenderModule::init()
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(m_displayModule->getNativeDisplay());
        ::bgfx::Init init;
        init.type = ::bgfx::RendererType::Count; // auto choose renderer (DirectX, OpenGL, etc.)
        init.platformData.nwh = glfwGetWin32Window(window);
        init.resolution.width = 800;
        init.resolution.height = 600;
        init.resolution.reset = BGFX_RESET_VSYNC;
        ::bgfx::init(init);
    }

    void BgfxRenderModule::beginFrame()
    {
        AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::endFrame()
    {
        ::bgfx::frame();
    }

    void BgfxRenderModule::drawText(string texto, float x, float y , float scale, Color color)
    {

    }

    void BgfxRenderModule::dispose()
    {
        AIKO_DEBUG_BREAK
    }

    texture::RenderTexture2D* BgfxRenderModule::getRenderTexture()
    {
        return &m_screenFbo.renderTexture;
    }

    void BgfxRenderModule::onWindowResize(Event& event)
    {

        RenderModule::onWindowResize(event);

        const auto& msg = static_cast<const WindowResizeEvent&>(event);

        if (m_scale == true)
        {
            return;
        }

        const auto screenWidth = msg.width;
        const auto screenHeight = msg.height;

        ::bgfx::setViewRect(0, 0, 0, screenWidth, screenHeight);

    }

    void BgfxRenderModule::clearBackground(Color color)
    {
        // FIXME 
        ::bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
    }

    void BgfxRenderModule::beginMode2D()
    {
        AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::endMode2D()
    {
        AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::beginMode3D()
    {
        AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::endMode3D()
    {
        AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::beginTextureMode()
    {

        AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::beginTextureMode(texture::RenderTexture2D& target)
    {
        AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::endTextureMode(void)
    {
        AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::beginShaderMode(aiko::Shader* shader)
    {
        AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::endShaderMode(void)
    {
        AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::beginBlendMode(BlendMode mode)
    {
        AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::endBlendMode(void)
    {
        AIKO_DEBUG_BREAK
    }

}
#endif