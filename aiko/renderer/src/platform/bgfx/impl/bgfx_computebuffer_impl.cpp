#include "bgfx_computebuffer_impl.h"

#include <cstring>

#include <logger/logger.h>
#include <core/file.h>

#include "platform/bgfx/bgfx_platform_helper.h"

namespace aiko::renderer::bgfx
{
    BgfxComputeBufferImpl::BgfxComputeBufferImpl()
        : m_handle({::bgfx::kInvalidHandle})
    {
    }

    BgfxComputeBufferImpl::~BgfxComputeBufferImpl()
    {
        destroy();
    }

    bool BgfxComputeBufferImpl::isValid() const
    {
        return ::bgfx::isValid(m_handle);
    }

    void BgfxComputeBufferImpl::create(ComputeBufferFormat format, uint32_t count, const void* initialData, ComputeAccess access)
    {

        destroy();

        m_format      = format;
        m_count       = count;
        m_strideBytes = getStrideBytesFor(format);

        buildLayout(format); // must produce a valid VertexLayout

        uint16_t flags = 0;
        switch (access)
        {
        case ComputeAccess::Read:      flags = BGFX_BUFFER_COMPUTE_READ;       break;
        case ComputeAccess::Write:     flags = BGFX_BUFFER_COMPUTE_WRITE;      break;
        case ComputeAccess::ReadWrite: flags = BGFX_BUFFER_COMPUTE_READ_WRITE; break;
        default: AIKO_ASSERT(false, "Unknown ComputeAccess"); break;
        }

        const bool gpuWritable = (access == ComputeAccess::Write) || (access == ComputeAccess::ReadWrite);

        if (gpuWritable)
        {
            // bgfx rule: can't initialize compute-write buffers from CPU.
            // Ignore initialData even if provided.
            m_handle = ::bgfx::createDynamicVertexBuffer(count, m_layout, flags);
        }
        else
        {
            // ComputeAccess::Read only: CPU initialization is allowed.
            const ::bgfx::Memory* mem = initialData
                ? ::bgfx::copy(initialData, count * m_strideBytes)
                : ::bgfx::alloc(count * m_strideBytes);

            m_handle = ::bgfx::createDynamicVertexBuffer(mem, m_layout, flags);
        }

        printf("stride = %u\n", m_strideBytes);

    }

    void BgfxComputeBufferImpl::update(uint32_t start, uint32_t count, const void* data)
    {
       AIKO_ASSERT(false, "Compute buffers are GPU-owned. Use staging buffer/readback system later.");
    }

    void BgfxComputeBufferImpl::destroy()
    {
        if (::bgfx::isValid(m_handle))
        {
            logger::Log::info("Destroy compute VB handle idx=%u", m_handle.idx);
            ::bgfx::destroy(m_handle);
            m_handle = {::bgfx::kInvalidHandle};
        }
    }

    void BgfxComputeBufferImpl::buildLayout(ComputeBufferFormat format)
    {
        m_layout = ::bgfx::VertexLayout{};
        m_layout.begin();

        switch (format)
        {
        case ComputeBufferFormat::Vec4f:
            // 1x vec4 in TEXCOORD0
            m_layout.add(::bgfx::Attrib::TexCoord0, 4, ::bgfx::AttribType::Float);
            break;
        default:
            // fallback
            m_layout.add(::bgfx::Attrib::TexCoord0, 4, ::bgfx::AttribType::Float);
            break;
        }

        m_layout.end();
    }

    u32 BgfxComputeBufferImpl::getStrideBytesFor(ComputeBufferFormat fmt)
    {
        switch (fmt)
        {
            case ComputeBufferFormat::Vec4f: return 16; // 4 floats
            default: return 16;
        }
        AIKO_ASSERT(false, "Not supported format")
    }
}
