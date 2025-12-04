#pragma once

#include <aiko_types.h>
#include <bgfx/bgfx.h>

#include "interfaces/iframebuffer_impl.h"

namespace aiko::bgfx
{

    class BgfxFrameBufferImpl : public interfaces::IFrameBufferImpl
    {
    public:
        BgfxFrameBufferImpl();

        virtual ~BgfxFrameBufferImpl() override = default;

        virtual uint id() const override;

        virtual void use() override;

        virtual void unuse() override;

        virtual bool isValid() const override;

        // load
        virtual void create(int width, int height) override;

        virtual void unload() override;

        ::bgfx::TextureHandle getColorTextureHandler() const { return m_colorTexture; }
        ::bgfx::TextureHandle getDepthTextureHandler() const { return m_depthTexture; }
        ::bgfx::FrameBufferHandle getFrameBufferHandler() const { return m_frameBuffer; }

    private:
        ::bgfx::TextureHandle m_colorTexture;
        ::bgfx::TextureHandle m_depthTexture;
        ::bgfx::FrameBufferHandle m_frameBuffer;
    };
}
