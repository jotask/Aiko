#pragma once

#include <aiko_types.h>

#include "frame_buffer.h"

namespace aiko
{
    class RenderTarget
    {
    public:
        RenderTarget() = default;
        ~RenderTarget() = default;

        RenderTarget(const RenderTarget&) = delete;
        RenderTarget& operator=(const RenderTarget&) = delete;

        RenderTarget(RenderTarget&&) noexcept = delete;
        RenderTarget& operator=(RenderTarget&&) noexcept = delete;

        bool isValid() const;

        void create(u32 width, u32 height);
        void resize(u32 width, u32 height);
        void unload();

        const FrameBuffer& frameBuffer() const
        {
            return m_frameBuffer;
        }

        FrameBuffer& frameBuffer()
        {
            return m_frameBuffer;
        }

        const Texture& colorTexture() const
        {
            return m_frameBuffer.getColorTexture();
        }

        const Texture& depthTexture() const
        {
            return m_frameBuffer.getDepthTexture();
        }

    private:
        FrameBuffer m_frameBuffer;
    };
}
