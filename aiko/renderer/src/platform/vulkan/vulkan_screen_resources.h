#pragma once

#include <cstdint>
#include <vector>

#include <volk.h>

namespace aiko::renderer::vulkan
{
    class VulkanContext;

    class VulkanScreenResources final
    {
    public:
        VulkanScreenResources(const VulkanScreenResources&) = delete;
        VulkanScreenResources& operator=(const VulkanScreenResources&) = delete;
        VulkanScreenResources(VulkanScreenResources&&) = delete;
        VulkanScreenResources& operator=(VulkanScreenResources&&) = delete;

        VulkanScreenResources(VulkanContext& context,uint32_t frameCount);

        void create();
        void destroy();

        void recreatePipeline();

        VkDescriptorSet descriptorSet( uint32_t frameIndex, VkImageView imageView, VkSampler sampler);

        VkPipeline pipeline() const
        {
            return m_pipeline;
        }

        VkPipelineLayout layout() const
        {
            return m_pipelineLayout;
        }

    private:
        void createDescriptorResources();
        void createPipelineLayout();
        void createPipeline();
        void destroyPipeline();

        VulkanContext& m_context;

        std::vector<VkDescriptorSet> m_descriptorSets;

        VkDescriptorSetLayout  m_descriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    };
}
