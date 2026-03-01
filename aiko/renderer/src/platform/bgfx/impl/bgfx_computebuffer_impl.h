#pragma once

#include <bgfx/bgfx.h>

#include "interfaces/i_computebuffer.h"

namespace aiko::renderer::bgfx
{

    class BgfxComputeBufferImpl : public interfaces::IComputeBufferImpl
    {
    public:

        BgfxComputeBufferImpl() = default;
        virtual ~BgfxComputeBufferImpl() override = default;

        virtual bool isValid() const override;

        virtual void create(ComputeBufferFormat format, uint32_t count, const void* initialData) override;
        virtual void update(uint32_t start, uint32_t count, const void* data) override;

        virtual void destroy() override;

        ::bgfx::DynamicVertexBufferHandle handle() const { return m_handle; }
        uint32_t count() const { return m_count; }
        ComputeBufferFormat format() const { return m_format; }

    private:

        void buildLayout(ComputeBufferFormat format);

        ::bgfx::DynamicVertexBufferHandle m_handle;
        ::bgfx::VertexLayout m_layout;

        ComputeBufferFormat m_format = ComputeBufferFormat::Vec4f;
        uint32_t m_count = 0;
        uint32_t m_strideBytes = 0;

    };
}
