#include "bgfx_framebuffer_impl.h"

#include <random>

#include <logger/logger.h>
#include <core/file.h>

#include <bgfx/bgfx.h>

namespace aiko::bgfx
{

    BgfxFrameBufferImpl::BgfxFrameBufferImpl()
        : m_frameBuffer({::bgfx::kInvalidHandle})
        , m_colorTexture({::bgfx::kInvalidHandle})
        , m_depthTexture({::bgfx::kInvalidHandle})
    {
    }

    uint BgfxFrameBufferImpl::id() const
    {
        return m_frameBuffer.idx;
    }

    void BgfxFrameBufferImpl::use()
    {

    }

    void BgfxFrameBufferImpl::unuse()
    {

    }

    bool BgfxFrameBufferImpl::isValid() const
    {
        return ::bgfx::isValid(m_colorTexture) && ::bgfx::isValid(m_depthTexture) && ::bgfx::isValid(m_frameBuffer);
    }

    void BgfxFrameBufferImpl::create(int width, int height)
    {

        m_colorTexture = ::bgfx::createTexture2D(
            width, height,         // width, height
            false,                 // hasMips
            1,                     // numLayers
            ::bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_RT        // mark as render target
        );

        // Example: create a depth buffer texture
        m_depthTexture = ::bgfx::createTexture2D(
            width, height,         // width, height
            false,
            1,
            ::bgfx::TextureFormat::D24S8,
            BGFX_TEXTURE_RT
        );

        std::vector<::bgfx::TextureHandle> fbTextures = { m_colorTexture, m_depthTexture };
        m_frameBuffer = ::bgfx::createFrameBuffer(fbTextures.size(), fbTextures.data(), true);
    }

    void BgfxFrameBufferImpl::unload()
    {
        ::bgfx::destroy(m_colorTexture);
        ::bgfx::destroy(m_depthTexture);
        ::bgfx::destroy(m_frameBuffer);
    }

}
