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
#include "models/camera.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"
#include "modules/render_primitives.h"
#include "core/libs.h"
#include "core/log.h"
#include "constants.h"

#include "modules/render_primitives.h"

#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#ifdef _WIN32
#include <GLFW/glfw3native.h>
#endif
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace aiko::fx
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
        RenderModule::init();
        bgfx::Init init;
        const auto size = getDisplaySize();
        init.resolution.width = size.x;
        init.resolution.height = size.y;
        init.type = bgfx::RendererType::Count;
        GLFWwindow* window = static_cast<GLFWwindow*>(m_displayModule->getNativeDisplay());
        init.platformData.nwh = glfwGetWin32Window(window);

        if ( bgfx::init(init) == false )
        {
            std::cerr << "Failed to initialize BGFX!" << std::endl;
            assert(false, "failed to init bgfx");
            return;
        }

        bgfx::setDebug(true);

        bgfx::setViewClear(0
            , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
            , background_color.rgba()
            , 1.0f
            , 0
        );
    }

    void BgfxRenderModule::postInit()
    {

    }

    void BgfxRenderModule::beginFrame()
    {
        
    }

    void BgfxRenderModule::endFrame()
    {

        const auto size = getDisplaySize();

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(size.x), uint16_t(size.y));

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx::touch(0);

        // Use debug font to print information about this example.
        bgfx::dbgTextClear();

        const bgfx::Stats* stats = bgfx::getStats();

        bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");

        bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
        bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");

        bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters."
            , stats->width
            , stats->height
            , stats->textWidth
            , stats->textHeight
        );

        // Advance to next frame. Rendering thread will be kicked to
        // process submitted rendering primitives.
        bgfx::frame();

    }

    void BgfxRenderModule::drawText(string texto, float x, float y , float scale, Color color)
    {
        
    }

    void BgfxRenderModule::dispose()
    {
        bgfx::shutdown();
    }

    texture::RenderTexture2D* BgfxRenderModule::getRenderTexture()
    {
        return &m_screenFbo.renderTexture;
    }

    void BgfxRenderModule::onWindowResize(Event& event)
    {

        RenderModule::onWindowResize(event);

        const auto& msg = static_cast<const WindowResizeEvent&>(event);

    }

    void BgfxRenderModule::clearBackground(Color color)
    {
        
    }

    void BgfxRenderModule::beginMode2D()
    {
        
    }

    void BgfxRenderModule::endMode2D()
    {
        
    }

    void BgfxRenderModule::beginMode3D()
    {

    }

    void BgfxRenderModule::endMode3D()
    {

    }

    void BgfxRenderModule::beginTextureMode()
    {

    }

    void BgfxRenderModule::beginTextureMode(texture::RenderTexture2D& target)
    {

    }

    void BgfxRenderModule::endTextureMode(void)
    {

    }

    void BgfxRenderModule::beginShaderMode(aiko::Shader* shader)
    {
        
    }

    void BgfxRenderModule::endShaderMode(void)
    {

    }

    void BgfxRenderModule::beginBlendMode(BlendMode mode)
    {

    }

    void BgfxRenderModule::endBlendMode(void)
    {

    }

}
#endif