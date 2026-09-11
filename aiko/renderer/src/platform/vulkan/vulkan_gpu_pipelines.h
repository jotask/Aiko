#pragma once

#include <unordered_map>

#include <volk.h>

#include "vulkan_pipeline_types.h"

namespace aiko::renderer::vulkan
{
    class VulkanContext;
    class VulkanShaderImpl;

    class VulkanGpuPipelines final
    {
    public:
        VulkanGpuPipelines(const VulkanGpuPipelines&) = delete;
        VulkanGpuPipelines& operator=(const VulkanGpuPipelines&) = delete;
        VulkanGpuPipelines(VulkanGpuPipelines&&) = delete;
        VulkanGpuPipelines& operator=(VulkanGpuPipelines&&) = delete;

        explicit VulkanGpuPipelines(VulkanContext& context);

        void create(VkPipelineLayout layout);

        void destroy();

        VkPipeline getOrCreateInstanced(const GpuPipelineKey& key, VkRenderPass renderPass, const VulkanShaderImpl& shader);

        VkPipeline getOrCreateVertex(const GpuVertexPipelineKey& key, VkRenderPass renderPass, const VulkanShaderImpl& shader);

    private:
        VkPipeline createInstancedPipeline(VkRenderPass renderPass,const VulkanShaderImpl& shader);

        VkPipeline createVertexPipeline(VkRenderPass renderPass, VkPrimitiveTopology topology, const VulkanShaderImpl& shader);

        VulkanContext& m_context;

        VkPipelineLayout m_layout = VK_NULL_HANDLE;

        std::unordered_map<GpuPipelineKey, VkPipeline, GpuPipelineKeyHash> m_instancedPipelines;
        std::unordered_map<GpuVertexPipelineKey, VkPipeline, GpuVertexPipelineKeyHash> m_vertexPipelines;
    };
}
