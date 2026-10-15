#pragma once

#include "models/compute_buffer.h"
#include "models/compute_shader.h"
#include "types/draw_types.h"

#include <cstddef>
#include <cstdint>
#include <deque>

namespace aiko
{
    namespace renderer
    {
        class IRenderDevice;
    }

    class GpuInstanceSorter
    {
    public:
        void beginFrame();
        void prepare(renderer::IRenderDevice& device, GpuInstanceDrawDesc& draw, const mat4& view);
        void dispose();

    private:
        struct SortBuffer
        {
            ComputeBuffer buffer;
            uint32_t capacity = 0;
        };

        ComputeShader m_initShader;
        ComputeShader m_stepShader;
        std::deque<SortBuffer> m_buffers;
        size_t m_usedBuffers = 0;
    };
}
