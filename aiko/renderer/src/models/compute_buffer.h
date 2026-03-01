 #pragma once

#include <aiko_types.h>
#include "interfaces/i_computebuffer.h"

 namespace aiko
{
    namespace renderer
    {
        class RendererFactory;
    }

    class ComputeBuffer
    {
    public:

        friend class renderer::RendererFactory;

        // Copy
        ComputeBuffer(const ComputeBuffer&) = default;
        ComputeBuffer& operator=(const ComputeBuffer&) = default;

        // Move
        ComputeBuffer(ComputeBuffer&&) noexcept = default;
        ComputeBuffer& operator=(ComputeBuffer&&) noexcept = default;

        ComputeBuffer();
        ~ComputeBuffer();

        bool isValid() const;

        void* getImpl() const { return backend.get(); }

        void create(ComputeBufferFormat format, u32 count, const void* initialData = nullptr);
        void update(u32 start, u32 count, const void* data);

        void destroy();

        uint id();

        // helpers
        void createVec4(uint32_t count, const void* initialData=nullptr);

    private:

        AikoPtr<interfaces::IComputeBufferImpl> backend;

    };

}
