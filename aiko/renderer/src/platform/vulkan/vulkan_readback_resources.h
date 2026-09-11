#pragma once

#include <deque>
#include <vector>

#include <volk.h>

#include "types/compute_pass.h"

namespace aiko::renderer::vulkan
{
    class VulkanContext;

    struct VulkanReadbackAllocation
    {
        VkBuffer buffer = VK_NULL_HANDLE;
        uint32_t byteSize = 0;
    };

    class VulkanReadbackResources final
    {
    public:
        VulkanReadbackResources(const VulkanReadbackResources&) = delete;
        VulkanReadbackResources& operator=(const VulkanReadbackResources&) = delete;
        VulkanReadbackResources(VulkanReadbackResources&&) = delete;
        VulkanReadbackResources& operator=(VulkanReadbackResources&&) = delete;

        explicit VulkanReadbackResources(VulkanContext& context);

        VulkanReadbackAllocation allocate(ReadbackId id, uint32_t byteSize, uint32_t frameIndex);

        void completeFrame(uint32_t frameIndex);

        bool poll(ComputeReadbackResult& result);

        void destroy();

    private:
        struct InFlightReadback
        {
            ReadbackId id = InvalidReadbackId;
            VkBuffer stagingBuffer = VK_NULL_HANDLE;
            VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
            uint32_t byteSize = 0;
            uint32_t frameIndex = 0;
        };

        struct CompletedReadback
        {
            ReadbackId id = InvalidReadbackId;
            vector<uint8_t> data;
        };

        VulkanContext& m_context;

        std::vector<InFlightReadback> m_inFlight;

        std::deque<CompletedReadback> m_completed;
    };
}
