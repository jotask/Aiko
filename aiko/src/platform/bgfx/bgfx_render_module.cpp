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
#include <bx/bx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "platform/bgfx/bgfx_platform_helper.h"
#include "platform/bgfx/bgfx_render_utils.h"

namespace aiko::bgfx
{

    BgfxRenderModule::BgfxRenderModule(Aiko* aiko)
        : RenderModule(aiko)
        , m_kViewMain(0)
        , m_kViewOffScreen(1)
        , currentViewId(m_kViewMain)
    {
        static_assert(sizeof(::bgfx::ViewId) == sizeof(BgfxRenderModule::ViewId),    "Bgfx ViewId type has changed");
        static_assert(std::is_same<::bgfx::ViewId, BgfxRenderModule::ViewId>::value, "Bgfx ViewId type has changed");
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

            uint32_t vendorId = ::bgfx::getCaps()->vendorId;
            uint32_t deviceId = ::bgfx::getCaps()->deviceId;
            Log::info() << "GPU: " << getVendorName(vendorId) << ", Vendor ID: 0x" << std::hex << vendorId << ", Device ID: 0x" << deviceId<< std::dec;
            Log::info() << "BGFX Renderer: " << ::bgfx::getRendererName(::bgfx::getRendererType());
        }

        ::aiko::bgfx::shared::init();

        initScreenFbo();

    }

    void BgfxRenderModule::beginFrame()
    {
        // Set view to the fbo
        currentViewId = m_kViewOffScreen;
        ::bgfx::setViewFrameBuffer(currentViewId, AIKO_TO_FBH(m_screenFbo.renderTexture.framebuffer));
        const auto size = m_displayModule->getCurrentDisplay().getDisplaySize();
        ::bgfx::setViewRect(currentViewId, 0, 0, size.x, size.y);
        clearBackground(background_color);
        ::bgfx::touch(currentViewId);
    }

    void BgfxRenderModule::endFrame()
    {

        currentViewId = m_kViewMain;

        const auto size = m_displayModule->getCurrentDisplay().getDisplaySize();

        ::bgfx::setViewRect(currentViewId, 0, 0, size.x, size.y);
        clearBackground(background_color);
        ::bgfx::touch(currentViewId);

        // Bind the offscreen texture to a sampler
        const ::bgfx::UniformHandle sampler = AIKO_TO_UH(m_passthrought.getUniformLocation("u_texture"));

        ::bgfx::setTexture(0, sampler, AIKO_TO_TH(m_screenFbo.renderTexture.texture.id));
        
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

        ::bgfx::submit(currentViewId, AIKO_TO_PH(m_passthrought.getData()->id));

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

        const uint16_t screenWidth = msg.width;
        const uint16_t screenHeight = msg.height;

        ::bgfx::reset((uint32_t)screenWidth, (uint32_t)screenHeight, BGFX_RESET_VSYNC);

        // FIXME is this correct?
        ::bgfx::setViewRect(m_kViewMain, 0, 0, screenWidth, screenHeight);
        ::bgfx::setViewRect(m_kViewOffScreen, 0, 0, screenWidth, screenHeight);

    }

    void BgfxRenderModule::clearBackground(Color color)
    {
        ::bgfx::setViewClear(currentViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, convertColorToBgfx(color), 1.0f, 0);
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