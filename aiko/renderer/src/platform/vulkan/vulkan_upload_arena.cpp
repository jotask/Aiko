#include "vulkan_upload_arena.h"

#include <limits>
#include <array>

#include "vulkan_context.h"

namespace aiko::renderer::vulkan
{
    VulkanUploadArena::VulkanUploadArena(VulkanContext& context, uint32_t frameCount)
        : m_context(context)
        , m_chunks(frameCount)
    {
        AIKO_ASSERT(frameCount > 0, "Vulkan upload arena requires at least one frame");
    }

    UploadSlice VulkanUploadArena::allocate(uint32_t frameIndex, VkDeviceSize size, VkDeviceSize alignment)
    {
        AIKO_ASSERT(frameIndex < m_chunks.size(), "Invalid Vulkan upload arena frame index");
        AIKO_ASSERT(size > 0, "Cannot allocate empty Vulkan upload slice");
        AIKO_ASSERT(alignment > 0, "Invalid Vulkan upload alignment");
        AIKO_ASSERT( (alignment & (alignment - 1)) == 0,"Vulkan upload alignment must be a power of two");

        const auto alignUp = [](VkDeviceSize value, VkDeviceSize align) { return (value + align - 1) & ~(align - 1); };

        std::vector<UploadArenaChunk>& chunks = m_chunks[frameIndex];

        for (UploadArenaChunk& chunk : chunks)
        {
            const VkDeviceSize alignedOffset = alignUp(chunk.offset, alignment);

            if (alignedOffset > chunk.capacity || size > chunk.capacity - alignedOffset)
            {
                continue;
            }

            const UploadSlice slice =
            {
                .buffer = chunk.buffer,
                .offset = alignedOffset,
                .mapped = static_cast<uint8_t*>(chunk.mapped) + alignedOffset,
            };

            chunk.offset = alignedOffset + size;

            return slice;
        }

        VkDeviceSize capacity = DefaultChunkSize;

        while (capacity < size)
        {
            AIKO_ASSERT(capacity <= std::numeric_limits<VkDeviceSize>::max() / 2, "Vulkan upload arena size overflow");
            capacity *= 2;
        }

        UploadArenaChunk chunk{};

        const VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        const VkMemoryPropertyFlags propertiesFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if (m_context.hasDedicatedComputeQueue())
        {
            const std::array<uint32_t, 2> queueFamilies =
            {
                m_context.graphicsQueueFamily(),
                m_context.computeQueueFamily(),
            };

            m_context.createBuffer(capacity, usageFlags, propertiesFlags, chunk.buffer, chunk.memory, VK_SHARING_MODE_CONCURRENT, queueFamilies.data(), static_cast<uint32_t>(queueFamilies.size()));
        }
        else
        {
            m_context.createBuffer(capacity, usageFlags, propertiesFlags, chunk.buffer, chunk.memory);
        }

        const VkResult mapResult = vkMapMemory(m_context.device(), chunk.memory, 0, capacity, 0, &chunk.mapped);
        AIKO_ASSERT(mapResult == VK_SUCCESS, "Failed to map Vulkan upload arena");

        chunk.capacity = capacity;
        chunk.offset = size;

        const UploadSlice slice =
        {
            .buffer = chunk.buffer,
            .offset = 0,
            .mapped = chunk.mapped,
        };

        chunks.push_back(chunk);

        return slice;
    }

    void VulkanUploadArena::resetFrame(uint32_t frameIndex)
    {
        AIKO_ASSERT(frameIndex < m_chunks.size(), "Invalid Vulkan upload arena frame index");
        for (UploadArenaChunk& chunk : m_chunks[frameIndex])
        {
            chunk.offset = 0;
        }
    }

    void VulkanUploadArena::destroy()
    {
        VkDevice device = m_context.device();

        for (auto& chunks : m_chunks)
        {
            for (UploadArenaChunk& chunk : chunks)
            {
                if (chunk.mapped != nullptr)
                {
                    vkUnmapMemory(device, chunk.memory);
                    chunk.mapped = nullptr;
                }

                if (chunk.buffer != VK_NULL_HANDLE)
                {
                    vkDestroyBuffer(device, chunk.buffer, nullptr);
                    chunk.buffer = VK_NULL_HANDLE;
                }

                if (chunk.memory != VK_NULL_HANDLE)
                {
                    vkFreeMemory(device, chunk.memory, nullptr);
                    chunk.memory = VK_NULL_HANDLE;
                }

                chunk.capacity = 0;
                chunk.offset = 0;
            }

            chunks.clear();
        }
    }
}
