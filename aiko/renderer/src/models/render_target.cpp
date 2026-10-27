#include "render_target.h"

#include <metadata/frame_buffer_meta.h>

namespace aiko
{
    bool RenderTarget::isValid() const
    {
        return m_frameBuffer.isValid();
    }

    void RenderTarget::create(u32 width, u32 height)
    {
        create(width, height, TextureFormat::RGBA8);
    }

    void RenderTarget::create(u32 width, u32 height, TextureFormat colorFormat)
    {
        AIKO_ASSERT( width > 0 && height > 0, "RenderTarget dimensions must be greater than zero");

        const FrameBufferDesc frameBufferDesc =
        {
            .width = static_cast<int>(width),
            .height = static_cast<int>(height),
            .colorFormat = colorFormat
        };

        m_frameBuffer.create(frameBufferDesc);

        m_colorFormat = colorFormat;

        if (!m_frameBuffer.isValid())
        {
            m_size = {0, 0};
            return;
        }

        m_size = {
            static_cast<int>(width),
            static_cast<int>(height)
        };
    }

    void RenderTarget::resize(u32 width, u32 height)
    {
        create(width, height, m_colorFormat);
    }

    void RenderTarget::unload()
    {
        m_frameBuffer.unload();
        m_size = {0, 0};
        m_colorFormat = TextureFormat::RGBA8;
    }
}
