#ifdef AIKO_BGFX

#include "bgfx_render_module.h"

#include <fstream>
#include <filesystem>

#include "core/libs.h"

#include "modules/module_connector.h"
#include "modules/display_module.h"
#include "models/camera.h"
#include "types/textures.h"
#include "events/events.hpp"
#include "types/render_types.h"
#include "models/mesh.h"
#include "models/shader.h"
#include "components/transform_component.h"
#include "constants.h"
#include "platform/bgfx/bgfx_platform_helper.h"

#include <bgfx/bgfx.h>


namespace aiko::bgfx
{

    void BgfxRenderModule::initScreenFbo()
    {

        // Postprocessing shader
        m_passthrought.load("posprocessing");
        assert(m_passthrought.isvalid() && "Processing shader invalid!");

        // VAO// IBH -> Not Needed we are rendering to full 

        // FBO

        auto size = m_displayModule->getCurrentDisplay().getDisplaySize();

        // Example: create a 2D color texture
        ::bgfx::TextureHandle colorTex = ::bgfx::createTexture2D(
            size.x, size.y,             // width, height
            false,                 // hasMips
            1,                     // numLayers
            ::bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_RT        // mark as render target
        );

        // Example: create a depth buffer texture
        ::bgfx::TextureHandle depthTex = ::bgfx::createTexture2D(
            size.x, size.y,
            false,
            1,
            ::bgfx::TextureFormat::D24S8,
            BGFX_TEXTURE_RT
        );

        std::vector<::bgfx::TextureHandle> fbTextures = { colorTex, depthTex };
        ::bgfx::FrameBufferHandle myFbo = ::bgfx::createFrameBuffer(fbTextures.size(), fbTextures.data(), true);

        texture::RenderTexture2D& target = m_screenFbo.renderTexture;
        target.framebuffer = myFbo.idx;

        target.texture.id = colorTex.idx;
        target.texture.width = size.x;
        target.texture.height = size.y;
        target.texture.format = ::bgfx::TextureFormat::RGBA8;

        target.depth.id = depthTex.idx;
        target.depth.width = size.x;
        target.depth.height = size.y;
        target.depth.format = ::bgfx::TextureFormat::D24S8;

    }

    texture::ScreenFbo BgfxRenderModule::getScreenFbo()
    {
        return m_screenFbo;
    }

}
#endif