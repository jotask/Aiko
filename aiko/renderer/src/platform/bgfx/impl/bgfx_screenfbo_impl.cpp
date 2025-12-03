#include "bgfx_screenfbo_impl.h"

#include <logger/logger.h>

#include <bgfx/bgfx.h>

namespace aiko::bgfx
{

    BgfxScreenFboImpl::BgfxScreenFboImpl()
        : m_frameBuffer()
        , vao (0)
        , vbo (0)
    {
    }

    uint BgfxScreenFboImpl::id() const
    {
        return m_frameBuffer->id();
    }

    void BgfxScreenFboImpl::use()
    {

    }

    void BgfxScreenFboImpl::unuse()
    {

    }

    bool BgfxScreenFboImpl::isValid() const
    {
        // TODO
        return false;
    }

    void BgfxScreenFboImpl::create(int width, int height)
    {
        // TODO
    }

    void BgfxScreenFboImpl::unload()
    {
        // TODO
    }

}
