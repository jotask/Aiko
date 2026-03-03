#include "compute_buffer.h"

#include "render_factory.h"

namespace aiko
{

    ComputeBuffer::ComputeBuffer()
        : backend(renderer::RendererFactory::createComputeBufferImpl())
    {

    }

    ComputeBuffer::~ComputeBuffer()
    {
        destroy();
    }

    bool ComputeBuffer::isValid() const
    {
        return backend->isValid();
    }

    void ComputeBuffer::create(ComputeBufferFormat format, uint32_t count, const void* data, ComputeAccess access)
    {
        backend->create(format, count, data, access);
    }

    void ComputeBuffer::update(u32 start, u32 count, const void* data)
    {
        backend->update(start, count, data);
    }

    void ComputeBuffer::destroy()
    {
        backend->destroy();
    }

    void ComputeBuffer::createVec4(uint32_t count, const void* initialData, ComputeAccess access)
    {
        create(ComputeBufferFormat::Vec4f, count, initialData, access);
    }
}
