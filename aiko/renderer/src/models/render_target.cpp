#include "render_target.h"

namespace aiko
{
    bool RenderTarget::isValid() const
    {
        return m_frameBuffer.isValid();
    }

    void RenderTarget::create(u32 width, u32 height)
    {
        AIKO_ASSERT(width > 0, "RenderTarget width must be greater than zero");
        AIKO_ASSERT(height > 0, "RenderTarget height must be greater than zero");
        m_frameBuffer.create(static_cast<int>(width), static_cast<int>(height));
        AIKO_ASSERT(m_frameBuffer.isValid(), "RenderTarget framebuffer invalid");
        m_size =
        {
            static_cast<int>(width),
            static_cast<int>(height)
        };
    }

    void RenderTarget::resize(u32 width, u32 height)
    {
        create(width, height);
    }

    void RenderTarget::unload()
    {
        m_frameBuffer.unload();
        m_size = {0, 0};
    }
}
