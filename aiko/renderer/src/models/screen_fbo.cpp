#include "screen_fbo.h"

#include "render_factory.h"

namespace aiko
{

    ScreenFbo::ScreenFbo()
        : backend(renderer::RendererFactory::createScreenFboImpl())
    {

    }

    void ScreenFbo::use()
    {
        backend->use();
    }

    void ScreenFbo::unuse()
    {
        backend->unuse();
    }

    bool ScreenFbo::isValid()
    {
        return backend->isValid();
    }

    uint ScreenFbo::id() const
    {
        return backend->id();
    }

    void ScreenFbo::create(int width, int height)
    {
        backend->create(width, height);
    }

    void ScreenFbo::unload()
    {
        backend->unload();
    }

    ivec2 ScreenFbo::getViewportSize() const
    {
        return backend->getViewportSize();
    }

    FrameBuffer ScreenFbo::getFrameBuffer() const
    {
        return backend->getFrameBuffer();
    }
}
