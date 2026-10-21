#include "gpu_instance_sorter.h"

#include "models/material.h"
#include "renderer/Irenderdevice.h"
#include "types/compute_pass.h"

#include <array>
#include <cstring>
#include <limits>

namespace aiko
{
    void GpuInstanceSorter::beginFrame()
    {
        m_usedBuffers = 0;
    }

    void GpuInstanceSorter::prepare(renderer::IRenderDevice& device, GpuInstanceDrawDesc& draw, const mat4& view, const IResourceResolver& resources)
    {
        if (draw.transparencyPositions == nullptr || draw.instanceCount == 0)
        {
            return;
        }

        AIKO_ASSERT(draw.material != nullptr, "Sorted GPU draw has no material");
        AIKO_ASSERT(draw.material->m_renderState.blend, "Instance sorting requires blending");
        AIKO_ASSERT(draw.transparencyPositions->isValid(), "Invalid transparency position buffer");

        for (const GpuReadBufferBinding& binding : draw.readBuffers)
        {
            AIKO_ASSERT(binding.slot != GpuInstanceOrderBinding, "Sorted GPU draws reserve binding 15 for instance order");
        }

        uint32_t paddedCount = 1;

        while (paddedCount < draw.instanceCount)
        {
            AIKO_ASSERT(paddedCount <= std::numeric_limits<uint32_t>::max() / 2, "GPU instance sort count exceeds supported range");

            paddedCount *= 2;
        }

        if (!m_initShader.isValid())
        {
            const auto init = resources.resolve("renderer://gpu_instance_sort_init.comp.spv");
            AIKO_ASSERTF(init, "GPU sort init shader missing: %s", init.message.c_str());
            m_initShader.load(init.path.string());
        }

        if (!m_stepShader.isValid())
        {
            const auto init = resources.resolve("renderer://gpu_instance_sort_step.comp.spv");
            AIKO_ASSERTF(init, "GPU sort step shader missing: %s", init.message.c_str());
            m_stepShader.load(init.path.string());
        }

        AIKO_ASSERT(m_initShader.isValid(), "Invalid instance sort initialization shader");
        AIKO_ASSERT(m_stepShader.isValid(), "Invalid instance sort step shader");

        if (m_usedBuffers == m_buffers.size())
        {
            m_buffers.emplace_back();
        }

        SortBuffer& output = m_buffers[m_usedBuffers++];

        if (output.capacity < paddedCount)
        {
            const ComputeBufferDesc description{
                .format = ComputeBufferFormat::Structured,
                .count = paddedCount,
                .stride = sizeof(uint32_t) * 2,
                .usage = ComputeBufferUsage::Storage
            };

            output.buffer.create(description);
            output.capacity = paddedCount;
        }

        struct alignas(16) InitParameters
        {
            std::array<float, 16> view{};
            uint32_t instanceCount = 0;
            uint32_t paddedCount = 0;
            uint32_t reserved0 = 0;
            uint32_t reserved1 = 0;
        };

        static_assert(sizeof(InitParameters) == 80);

        InitParameters initialize{};
        std::memcpy(initialize.view.data(), view.data(), sizeof(float) * 16);
        initialize.instanceCount = draw.instanceCount;
        initialize.paddedCount = paddedCount;

        const uint32_t groups = (paddedCount - 1) / 256 + 1;

        ComputePass init{};
        init.shader = &m_initShader;
        init.buffers = {
            {0, draw.transparencyPositions, ComputeAccess::Read},
            {1, &output.buffer, ComputeAccess::Write}
        };
        init.dispatch.groupsX = groups;
        init.setPushConstants(initialize);

        device.execute(COMPUTE_VIEW, init);

        struct StepParameters
        {
            uint32_t paddedCount;
            uint32_t sequenceLength;
            uint32_t comparisonDistance;
        };

        static_assert(sizeof(StepParameters) == 12);

        ComputePass step{};
        step.shader = &m_stepShader;
        step.buffers = {
            {0, &output.buffer, ComputeAccess::ReadWrite}
        };
        step.dispatch.groupsX = groups;

        for (uint32_t sequence = 2; sequence <= paddedCount;)
        {
            for (uint32_t distance = sequence / 2; distance != 0; distance /= 2)
            {
                const StepParameters parameters{
                    paddedCount,
                    sequence,
                    distance
                };

                step.setPushConstants(parameters);
                device.execute(COMPUTE_VIEW, step);
            }

            if (sequence == paddedCount)
            {
                break;
            }

            sequence *= 2;
        }

        draw.readBuffers.push_back({ GpuInstanceOrderBinding, &output.buffer });
    }

    void GpuInstanceSorter::dispose()
    {
        m_buffers.clear();
        m_usedBuffers = 0;
        m_stepShader.unload();
        m_initShader.unload();
    }
}