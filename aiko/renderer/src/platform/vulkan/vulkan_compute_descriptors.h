#pragma once

#include <cstdint>
#include <vector>

#include <volk.h>

namespace aiko::renderer::vulkan
{
    class VulkanContext;

    class VulkanComputeDescriptors final
    {
    public:

        VulkanComputeDescriptors(const VulkanComputeDescriptors&) = delete;
        VulkanComputeDescriptors& operator=(const VulkanComputeDescriptors&) = delete;
        VulkanComputeDescriptors(VulkanComputeDescriptors&&) = delete;
        VulkanComputeDescriptors& operator=(VulkanComputeDescriptors&&) = delete;

        VulkanComputeDescriptors(VulkanContext& context, uint32_t frameCount);

        void create();
        void destroy();

        void resetFrame(uint32_t frameIndex);

        VkDescriptorSet allocate(uint32_t frameIndex);

        VkDescriptorSetLayout layout() const
        {
            return m_layout;
        }

    private:
        static constexpr uint32_t MaxDispatchesPerFrame = 256;

        VulkanContext& m_context;

        std::vector<VkDescriptorPool> m_pools;

        VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
    };
}
