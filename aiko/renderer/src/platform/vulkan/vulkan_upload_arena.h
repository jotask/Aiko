#pragma once

#include <cstdint>
#include <vector>

#include "vulkan_transfer_types.h"

namespace aiko::renderer::vulkan
{
    class VulkanContext;

    class VulkanUploadArena final
    {
    public:

        VulkanUploadArena(const VulkanUploadArena&) = delete;
        VulkanUploadArena& operator=(const VulkanUploadArena&) = delete;
        VulkanUploadArena(VulkanUploadArena&&) = delete;
        VulkanUploadArena& operator=(VulkanUploadArena&&) = delete;

        VulkanUploadArena(VulkanContext& context, uint32_t frameCount);

        UploadSlice allocate(uint32_t frameIndex, VkDeviceSize size, VkDeviceSize alignment);

        void resetFrame(uint32_t frameIndex);
        void destroy();

    private:
        static constexpr VkDeviceSize DefaultChunkSize = 4 * 1024 * 1024;

        VulkanContext& m_context;

        std::vector<std::vector<UploadArenaChunk>> m_chunks;
    };
}
