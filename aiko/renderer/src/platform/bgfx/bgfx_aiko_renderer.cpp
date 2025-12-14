#ifdef AIKO_BGFX

#include "bgfx_aiko_renderer.h"

#include <logger/logger.h>
#include <events/events.hpp>

#include "display/display_manager.h"
#include "display/display_events.hpp"

#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <GLFW/glfw3.h>

#include "impl/bgfx_screenfbo_impl.h"
#include "impl/bgfx_shader_impl.h"
#include "impl/bgfx_framebuffer_impl.h"

#if defined(AIKO_WINDOWS)
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#elif defined(AIKO_LINUX)
    #define GLFW_EXPOSE_NATIVE_X11
    #include <GLFW/glfw3native.h>
#else
    #error OS unsupported!
#endif

#include "platform/bgfx/bgfx_platform_helper.h"
#include "platform/bgfx/bgfx_types.h"

namespace aiko::bgfx
{

    BgfxRenderer::BgfxRenderer()
        : IAikoRenderer()
        , m_kViewMain(0)
        , m_kViewOffScreen(1)
        , currentViewId(m_kViewMain)
    {
        static_assert(sizeof(::bgfx::ViewId) == sizeof(BgfxRenderer::ViewId),    "Bgfx ViewId type has changed");
        static_assert(std::is_same<::bgfx::ViewId, BgfxRenderer::ViewId>::value, "Bgfx ViewId type has changed");
    }

    BgfxRenderer::~BgfxRenderer()
    {
        
    }

    void BgfxRenderer::init()
    {

        auto* window = DisplayManager::it().getNativeWindow();
        const auto size = DisplayManager::it().getDisplay()->getDisplaySize();

        ::bgfx::Init init;
        init.type = ::bgfx::RendererType::Count; // auto choose renderer (DirectX, OpenGL, etc.)
        #if defined(AIKO_WINDOWS)
            init.platformData.nwh = glfwGetWin32Window(window);
        #elif defined(AIKO_LINUX)
            init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
            init.platformData.ndt = glfwGetX11Display();
        #else
            #error OS unsupported!
        #endif
        init.resolution.width = size.x;
        init.resolution.height = size.y;
        init.resolution.reset = AIKO_VSYNC_MACRO;

        AIKO_ASSERT(::bgfx::init(init), "Failed to init BGFX")
        
        // Log Init
        {

            static const std::unordered_map<uint32_t, std::string> vendorMap = {
                { 0x10DE, "NVIDIA" },
                { 0x1002, "AMD/ATI" },
                { 0x8086, "Intel" },
                { 0x5143, "Qualcomm" },
                { 0x13B5, "ARM" },
                { 0x5333, "S3 Graphics" },
                { 0x102B, "Matrox" }
                // add more if needed
            };

            auto getVendorName = [&](uint32_t vendorId) -> std::string
                {
                    auto it = vendorMap.find(vendorId);
                    if (it != vendorMap.end())
                        return it->second;
                    return std::string{ "Unknown" };
                };

            const uint32_t vendorId = ::bgfx::getCaps()->vendorId;
            const uint32_t deviceId = ::bgfx::getCaps()->deviceId;
            logger::Log::info() << "GPU: " << getVendorName(vendorId) << ", Vendor ID: 0x" << std::hex << vendorId << ", Device ID: 0x" << deviceId<< std::dec;
            logger::Log::info() << "BGFX Renderer: " << ::bgfx::getRendererName(::bgfx::getRendererType());
        }

        // ::bgfx::setDebug(BGFX_DEBUG_WIREFRAME | BGFX_DEBUG_STATS | BGFX_DEBUG_TEXT);

        initScreenFbo();

    }

    void BgfxRenderer::beginFrame()
    {
        // Set view to the fbo
        currentViewId = m_kViewOffScreen;

        GET_BACKEND_IMPL(m_screenFbo.getImpl(), BgfxScreenFboImpl, screen);
        GET_BACKEND_IMPL(screen->getFrameBuffer().getImpl(), BgfxFrameBufferImpl, fbo);
        ::bgfx::setViewFrameBuffer(currentViewId, fbo->getFrameBufferHandler());

        const auto size = DisplayManager::it().getDisplay()->getDisplaySize();
        ::bgfx::setViewRect(currentViewId, 0, 0, size.x, size.y);
        clearBackground(m_background_color);
        ::bgfx::touch(currentViewId);
    }

    void BgfxRenderer::endFrame()
    {
        currentViewId = m_kViewMain;
        const auto size = DisplayManager::it().getDisplay()->getDisplaySize();
        ::bgfx::setViewRect(currentViewId, 0, 0, size.x, size.y);
        clearBackground(m_background_color);
        ::bgfx::touch(currentViewId);

        // Bind the offscreen texture to a sampler
        GET_BACKEND_IMPL(m_passThrough.getImpl(), BgfxShaderImpl, pass)
        const ::bgfx::UniformHandle sampler = pass->getUniformHandle("u_texture");

        GET_BACKEND_IMPL(m_screenFbo.getImpl(), BgfxScreenFboImpl, screen);
        GET_BACKEND_IMPL(screen->getFrameBuffer().getImpl(), BgfxFrameBufferImpl, fbo);
        ::bgfx::setTexture(0, sampler, fbo->getColorTextureHandler());
        
        auto screenSpaceQuad = [](float width, float heigh)
            {

                struct ScreenQuadVertex
                {
                    float x, y, z;
                    float u, v;
                };

                static ::bgfx::VertexLayout s_screenQuadLayout;
                s_screenQuadLayout.begin()
                    .add(::bgfx::Attrib::Position, 3,  ::bgfx::AttribType::Float)
                    .add(::bgfx::Attrib::TexCoord0, 2, ::bgfx::AttribType::Float)
                    .end();

                const bool _originBottomLeft = false;

                const uint16_t numVertices = 4;
                const uint16_t numIndices = 6;

                ::bgfx::TransientVertexBuffer vb;
                ::bgfx::allocTransientVertexBuffer(&vb, numVertices, s_screenQuadLayout);

                ::bgfx::TransientIndexBuffer ib;
                ::bgfx::allocTransientIndexBuffer(&ib, numIndices);

                const float size = 1.0f;

                const float minx = -size;
                const float maxx =  size;
                const float miny = -size;
                const float maxy =  size;

                const float minu = 0.0f;
                const float maxu = 1.0f;
                const float minv = _originBottomLeft ? 0.0f : 1.0f;
                const float maxv = _originBottomLeft ? 1.0f : 0.0f;

                ScreenQuadVertex* vertex = (ScreenQuadVertex*)vb.data;

                vertex[0] = { minx, miny, 0.0f, minu, minv };
                vertex[1] = { maxx, miny, 0.0f, maxu, minv };
                vertex[2] = { maxx, maxy, 0.0f, maxu, maxv };
                vertex[3] = { minx, maxy, 0.0f, minu, maxv };

                uint16_t* indices = (uint16_t*)ib.data;
                indices[0] = 0;
                indices[1] = 1;
                indices[2] = 2;
                indices[3] = 2;
                indices[4] = 3;
                indices[5] = 0;

                ::bgfx::setVertexBuffer(0, &vb);
                ::bgfx::setIndexBuffer(&ib);
            };

        screenSpaceQuad((float)size.x, (float)size.y);

        ::bgfx::submit(currentViewId, pass->getProgramHandler());

        ::bgfx::frame();

    }

    void BgfxRenderer::drawText(string texto, float x, float y , float scale, Color color)
    {

    }

    void BgfxRenderer::dispose()
    {
        ::bgfx::shutdown();
    }

    void BgfxRenderer::onWindowResize(Event& event)
    {

        const auto& msg = static_cast<const WindowResizeEvent&>(event);

        const uint16_t screenWidth = msg.width;
        const uint16_t screenHeight = msg.height;

        ::bgfx::reset((uint32_t)screenWidth, (uint32_t)screenHeight, AIKO_VSYNC_MACRO);

        // FIXME is this correct?
        ::bgfx::setViewRect(m_kViewMain, 0, 0, screenWidth, screenHeight);
        ::bgfx::setViewRect(m_kViewOffScreen, 0, 0, screenWidth, screenHeight);

        m_screenFbo.destroy();
        initScreenFbo();

    }

    void BgfxRenderer::initScreenFbo()
    {
        AIKO_NOT_IMPLEMENTED;
    }

    void BgfxRenderer::clearBackground(Color color)
    {
        ::bgfx::setViewClear(currentViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, convertColorToBgfx(color), 1.0f, 0);
    }

}
#endif