 #pragma once

#include <aiko_types.h>
#include "interfaces/i_computebuffer.h"

 namespace aiko
{
    namespace renderer
    {
        class RendererFactory;
        class IRenderDevice;
    }

    class ComputeBuffer
    {
        friend class renderer::IRenderDevice;
        friend class renderer::RendererFactory;
    public:

        // Copy
        ComputeBuffer(const ComputeBuffer&) = delete;
        ComputeBuffer& operator=(const ComputeBuffer&) = delete;

        // Move
        ComputeBuffer(ComputeBuffer&&) noexcept = delete;
        ComputeBuffer& operator=(ComputeBuffer&&) noexcept = delete;

        ComputeBuffer();
        ~ComputeBuffer();

        bool isValid() const;

        void create(const ComputeBufferDesc& desc, const void* initialData = nullptr);
        void update(u32 start, u32 count, const void* data);

        void unload();

        // helpers
        void createVec4(uint32_t count, const void* initialData);

    private:

        AikoPtr<interfaces::IComputeBufferImpl> backend;

    };

}
