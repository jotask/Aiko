#include "bgfx_computebuffer_impl.h"

#include <cstring>

#include <logger/logger.h>
#include <core/file.h>

#include "platform/bgfx/bgfx_platform_helper.h"

namespace aiko::renderer::bgfx
{
    bool BgfxComputeBufferImpl::isValid() const
    {
        return ::bgfx::isValid(m_handle);
    }

    void BgfxComputeBufferImpl::create(ComputeBufferFormat format, uint32_t count, const void* initialData)
    {

        auto strideBytesFor = [](ComputeBufferFormat fmt) -> uint32_t
        {
            switch (fmt)
            {
            case ComputeBufferFormat::Vec4f: return 16; // 4 floats
            default: return 16;
            }
        };

        destroy();

        m_format = format;
        m_count  = count;
        m_strideBytes = strideBytesFor(format);

        buildLayout(format);

        // Compute RW buffer: Dynamic VB + compute flag
        m_handle = ::bgfx::createDynamicVertexBuffer(
            count,
            m_layout,
            BGFX_BUFFER_COMPUTE_READ_WRITE
        );

        if (initialData != nullptr)
        {
            update(0, count, initialData);
        }

    }

    void BgfxComputeBufferImpl::update(uint32_t start, uint32_t count, const void* data)
    {

        if (!::bgfx::isValid(m_handle) || data == nullptr || count == 0)
            return;

        const uint32_t byteSize = count * m_strideBytes;

        const ::bgfx::Memory* mem = ::bgfx::alloc(byteSize);
        std::memcpy(mem->data, data, byteSize);

        ::bgfx::update(m_handle, start, mem);

    }

    void BgfxComputeBufferImpl::destroy()
    {
        if (::bgfx::isValid(m_handle))
        {
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
}
