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
        ComputeBuffer(const ComputeBuffer&) = delete;
        ComputeBuffer& operator=(const ComputeBuffer&) = delete;

        // Move
        ComputeBuffer(ComputeBuffer&&) noexcept = default;
        ComputeBuffer& operator=(ComputeBuffer&&) noexcept = default;

        ComputeBuffer();
        ~ComputeBuffer();

        bool isValid() const;

        void* getImpl() const { return backend.get(); }

        void create(ComputeBufferFormat format, u32 count, const void* initialData, ComputeAccess access);
        void update(u32 start, u32 count, const void* data);

        void unload();

        uint id();

        // helpers
        void createVec4(uint32_t count, const void* initialData, ComputeAccess access);

    private:

        AikoPtr<interfaces::IComputeBufferImpl> backend;

    };

}
