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

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "platform/bgfx/bgfx_platform_helper.h"

namespace aiko::bgfx
{

    BgfxRenderModule::BgfxRenderModule(Aiko* aiko)
        : RenderModule(aiko)
    {
        static_assert(sizeof(::bgfx::ViewId) == sizeof(BgfxRenderModule::ViewId) && "Bgfx ViewId type has changed");
    }

    BgfxRenderModule::~BgfxRenderModule()
    {
        
    }


    void BgfxRenderModule::preInit()
    {
        // FIXME
    }

    void BgfxRenderModule::init()
    {

        RenderModule::init();

        GLFWwindow* window = static_cast<GLFWwindow*>(m_displayModule->getNativeDisplay());
        const ivec2 displaySize = m_displayModule->getCurrentDisplay().getDisplaySize();
        ::bgfx::Init init;
        init.type = ::bgfx::RendererType::Count; // auto choose renderer (DirectX, OpenGL, etc.)
        init.platformData.nwh = glfwGetWin32Window(window);
        init.resolution.width = displaySize.x;
        init.resolution.height = displaySize.y;
        init.resolution.reset = BGFX_RESET_VSYNC;
        if (::bgfx::init(init) == false)
        {
            return std::exit(99);
        }

        auto caca = ::bgfx::getRendererType();
        m_kClearView = 0;
        ::bgfx::setViewClear(m_kClearView, BGFX_CLEAR_COLOR);
        ::bgfx::setViewRect(m_kClearView, 0, 0, ::bgfx::BackbufferRatio::Equal);

    }

    void BgfxRenderModule::beginFrame()
    {
        clearBackground(background_color);
        ::bgfx::touch(m_kClearView);
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
        ::bgfx::shutdown();
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

        ::bgfx::reset((uint32_t)screenWidth, (uint32_t)screenHeight, BGFX_RESET_VSYNC);
        ::bgfx::setViewRect(m_kClearView, 0, 0, ::bgfx::BackbufferRatio::Equal);

    }

    void BgfxRenderModule::clearBackground(Color color)
    {
        ::bgfx::setViewClear(m_kClearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, convertColorToBgfx(color), 1.0f, 0);
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
        //AIKO_DEBUG_BREAK
    }

    void BgfxRenderModule::endMode3D()
    {
        //AIKO_DEBUG_BREAK
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