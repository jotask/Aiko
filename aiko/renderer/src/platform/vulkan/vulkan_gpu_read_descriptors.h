#pragma once

#include <cstdint>
#include <vector>

#include <volk.h>

namespace aiko::renderer::vulkan
{
    class VulkanContext;

    class VulkanGpuReadDescriptors final
    {
    public:

        VulkanGpuReadDescriptors(const VulkanGpuReadDescriptors&) = delete;
        VulkanGpuReadDescriptors& operator=(const VulkanGpuReadDescriptors&) = delete;
        VulkanGpuReadDescriptors(VulkanGpuReadDescriptors&&) = delete;
        VulkanGpuReadDescriptors& operator=(VulkanGpuReadDescriptors&&) = delete;

        VulkanGpuReadDescriptors(VulkanContext& context, uint32_t frameCount);

        void create();
        void destroy();

        void resetFrame(uint32_t frameIndex);

        VkDescriptorSet allocate(uint32_t frameIndex);

        VkDescriptorSetLayout layout() const { return m_layout; }

    private:
        static constexpr uint32_t MaxSetsPerFrame = 256;

        VulkanContext& m_context;

        std::vector<VkDescriptorPool> m_pools;

        VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
    };
}
