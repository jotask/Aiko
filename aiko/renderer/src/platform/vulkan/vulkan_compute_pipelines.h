#pragma once

#include <unordered_map>

#include <volk.h>

namespace aiko::renderer::vulkan
{
    class VulkanContext;
    class VulkanComputeShaderImpl;

    class VulkanComputePipelines final
    {
    public:
        VulkanComputePipelines(const VulkanComputePipelines&) = delete;
        VulkanComputePipelines& operator=(const VulkanComputePipelines&) = delete;
        VulkanComputePipelines(VulkanComputePipelines&&) = delete;
        VulkanComputePipelines& operator=(VulkanComputePipelines&&) = delete;

        explicit VulkanComputePipelines(VulkanContext& context);

        void create(VkDescriptorSetLayout descriptorSetLayout);

        void destroy();

        VkPipeline getOrCreate(const VulkanComputeShaderImpl& shader);

        VkPipelineLayout layout() const { return m_layout; }

    private:
        VulkanContext& m_context;

        VkPipelineLayout m_layout = VK_NULL_HANDLE;

        std::unordered_map<VkShaderModule, VkPipeline> m_pipelines;
    };
}
