#include "vulkan_readback_resources.h"

#include <cstring>

#include "vulkan_context.h"

namespace aiko::renderer::vulkan
{
    VulkanReadbackResources::VulkanReadbackResources(VulkanContext& context)
        : m_context(context)
    {
    }

    VulkanReadbackAllocation VulkanReadbackResources::allocate(ReadbackId id, uint32_t byteSize, uint32_t frameIndex)
    {
        AIKO_ASSERT(byteSize > 0, "Vulkan readback size must be greater than zero");

        InFlightReadback readback{};

        readback.id = id;
        readback.byteSize = byteSize;
        readback.frameIndex = frameIndex;

        m_context.createBuffer(byteSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, readback.stagingBuffer, readback.stagingMemory);

        AIKO_ASSERT(readback.stagingBuffer != VK_NULL_HANDLE, "Failed to create Vulkan readback staging buffer");
        AIKO_ASSERT(readback.stagingMemory != VK_NULL_HANDLE, "Failed to allocate Vulkan readback staging memory");

        const VulkanReadbackAllocation allocation =
        {
            .buffer = readback.stagingBuffer,
            .byteSize = readback.byteSize,
        };

        m_inFlight.push_back(std::move(readback));

        return allocation;
    }

    void VulkanReadbackResources::completeFrame(uint32_t frameIndex)
    {
        VkDevice device = m_context.device();

        auto it = m_inFlight.begin();
        while (it != m_inFlight.end())
        {
            if (it->frameIndex != frameIndex)
            {
                ++it;
                continue;
            }

            CompletedReadback completed{};
            completed.id = it->id;
            completed.data.resize(it->byteSize);

            void* mapped = nullptr;

            const VkResult result = vkMapMemory(device, it->stagingMemory, 0, it->byteSize, 0, &mapped);
            AIKO_ASSERT(result == VK_SUCCESS, "Failed to map Vulkan readback staging buffer");

            std::memcpy(completed.data.data(), mapped, it->byteSize);

            vkUnmapMemory(device, it->stagingMemory);
            vkDestroyBuffer(device, it->stagingBuffer, nullptr);
            vkFreeMemory(device, it->stagingMemory, nullptr);

            m_completed.push_back(std::move(completed));
            it = m_inFlight.erase(it);
        }
    }

    bool VulkanReadbackResources::poll(ComputeReadbackResult& result)
    {
        if (m_completed.empty())
        {
            return false;
        }

        CompletedReadback completed = std::move(m_completed.front());
        m_completed.pop_front();
        result.id = completed.id;
        result.ready = true;
        result.data = std::move(completed.data);
        return true;
    }

    void VulkanReadbackResources::destroy()
    {
        VkDevice device = m_context.device();

        for (InFlightReadback& readback : m_inFlight)
        {
            if (readback.stagingBuffer != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(device, readback.stagingBuffer, nullptr);
            }

            if (readback.stagingMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, readback.stagingMemory, nullptr);
            }
        }

        m_inFlight.clear();
        m_completed.clear();
    }
}
