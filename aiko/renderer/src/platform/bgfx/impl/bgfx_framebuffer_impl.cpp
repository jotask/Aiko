#include "bgfx_framebuffer_impl.h"

#include <random>

#include <core/file.h>

#include "platform/bgfx/impl/bgfx_texture_impl.h"

namespace aiko::renderer::bgfx
{

    BgfxFrameBufferImpl::BgfxFrameBufferImpl()
        : m_frameBuffer({::bgfx::kInvalidHandle})
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
        return ::bgfx::isValid(m_frameBuffer);
    }

    void BgfxFrameBufferImpl::create(Texture& color, Texture& depth)
    {

        BgfxTextureImpl* colorTexture = static_cast<BgfxTextureImpl*>(color.getImpl());
        BgfxTextureImpl* depthTexture = static_cast<BgfxTextureImpl*>(depth.getImpl());

        AIKO_ASSERT(colorTexture->isValid(), "Invalid color texture")
        AIKO_ASSERT(depthTexture->isValid(), "Invalid depth texture")

        vector fbTextures = { colorTexture->getTextureHandler(), depthTexture->getTextureHandler() };
        m_frameBuffer = ::bgfx::createFrameBuffer(fbTextures.size(), fbTextures.data(), false);

    }

    void BgfxFrameBufferImpl::unload()
    {
        if (::bgfx::isValid(m_frameBuffer))
        {
            ::bgfx::destroy(m_frameBuffer);
        }
    }

}
