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
        unload();
    }

    bool ComputeBuffer::isValid() const
    {
        return backend->isValid();
    }

    void ComputeBuffer::create(const ComputeBufferDesc& desc, const void* initialData)
    {
        backend->create(desc, initialData);
    }

    void ComputeBuffer::update(u32 start, u32 count, const void* data)
    {
        backend->update(start, count, data);
    }

    void ComputeBuffer::unload()
    {
        backend->destroy();
    }

    void ComputeBuffer::createVec4(uint32_t count, const void* initialData)
    {
        const ComputeBufferDesc desc =
        {
            .format = ComputeBufferFormat::Vec4f,
            .count = count,
            .usage = ComputeBufferUsage::Storage | ComputeBufferUsage::TransferSrc | ComputeBufferUsage::TransferDst,
        };
        create(desc, initialData);
    }
}
