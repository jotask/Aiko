#include "frame_buffer.h"

#include "render_factory.h"

namespace aiko
{

    FrameBuffer::FrameBuffer()
        : backend(renderer::RendererFactory::createFrameBufferImpl())
    {

    }

    void FrameBuffer::use()
    {
        backend->use();
    }

    void FrameBuffer::unuse()
    {
        backend->unuse();
    }

    bool FrameBuffer::isValid()
    {
        return backend->isValid();
    }

    uint FrameBuffer::id() const
    {
        return backend->id();
    }

    void FrameBuffer::create(int width, int height)
    {
        backend->create(width, height);
    }

    void FrameBuffer::unload()
    {
        backend->unload();
    }

}
