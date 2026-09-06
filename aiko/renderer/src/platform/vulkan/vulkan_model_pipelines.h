#pragma once

#include <unordered_map>

#include <volk.h>

#include "vulkan_pipeline_types.h"

namespace aiko::renderer::vulkan
{
    class VulkanContext;
    class VulkanShaderImpl;

    class VulkanModelPipelines final
    {
    public:
        VulkanModelPipelines(const VulkanModelPipelines&) = delete;
        VulkanModelPipelines& operator=(const VulkanModelPipelines&) = delete;
        VulkanModelPipelines(VulkanModelPipelines&&) = delete;
        VulkanModelPipelines& operator=(VulkanModelPipelines&&) = delete;

        explicit VulkanModelPipelines(VulkanContext& context);

        void create(VkDescriptorSetLayout frameLayout, VkDescriptorSetLayout gpuReadLayout);

        void destroy();

        VkPipeline getOrCreate( const ModelPipelineKey& key, VkRenderPass renderPass, const VulkanShaderImpl& shader);
        VkPipeline getOrCreateInstanced(const ModelPipelineKey& key, VkRenderPass renderPass);

        VkPipelineLayout layout() const { return m_layout; }
        VkDescriptorSetLayout materialLayout() const { return m_materialLayout; }

    private:
        void createMaterialLayout();

        void createPipelineLayout(VkDescriptorSetLayout frameLayout, VkDescriptorSetLayout gpuReadLayout);

        VkPipeline createModelPipeline(const ModelPipelineKey& key, VkRenderPass renderPass, const VulkanShaderImpl& shader);

        VkPipeline createInstancedPipeline(const ModelPipelineKey& key, VkRenderPass renderPass);

        VulkanContext& m_context;

        VkDescriptorSetLayout m_materialLayout = VK_NULL_HANDLE;

        VkPipelineLayout m_layout = VK_NULL_HANDLE;

        std::unordered_map<ModelPipelineKey, VkPipeline, ModelPipelineKeyHash> m_pipelines;
    };
}
