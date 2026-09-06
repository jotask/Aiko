#include "vulkan_compute_pipelines.h"

#include "vulkan_context.h"
#include "vulkan_shader_reflector.h"
#include "impl/vulkan_computeshader_impl.h"

#include "types/compute_pass.h"

namespace aiko::renderer::vulkan
{
    VulkanComputePipelines::VulkanComputePipelines(VulkanContext& context)
        : m_context(context)
    {
    }

    void VulkanComputePipelines::create(VkDescriptorSetLayout descriptorSetLayout)
    {
        AIKO_ASSERT(descriptorSetLayout != VK_NULL_HANDLE, "Compute descriptor layout is invalid");
        AIKO_ASSERT(m_layout == VK_NULL_HANDLE, "Vulkan compute pipelines already created");

        const VkPushConstantRange pushConstantRange =
        {
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .offset = 0,
            .size = MaxComputePushConstantBytes,
        };

        const VkPipelineLayoutCreateInfo layoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 1,
            .pSetLayouts = &descriptorSetLayout,
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &pushConstantRange,
        };

        const VkResult result = vkCreatePipelineLayout(m_context.device(), &layoutInfo, nullptr, &m_layout);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create compute pipeline layout");

    }

    VkPipeline VulkanComputePipelines::getOrCreate(const VulkanComputeShaderImpl& shader)
    {
        AIKO_ASSERT(m_layout != VK_NULL_HANDLE,"Compute pipeline layout is invalid");

        const VkShaderModule shaderModule = shader.module();
        AIKO_ASSERT(shaderModule != VK_NULL_HANDLE, "Invalid compute shader module");

        const auto it = m_pipelines.find(shaderModule);

        if (it != m_pipelines.end())
        {
            return it->second;
        }

        validateComputePushConstants(shader.reflection(), MaxComputePushConstantBytes);

        const VkPipelineShaderStageCreateInfo shaderStage =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = shaderModule,
            .pName = "main",
        };

        const VkComputePipelineCreateInfo pipelineInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .stage = shaderStage,
            .layout = m_layout,
        };

        VkPipeline pipeline = VK_NULL_HANDLE;

        const VkResult result = vkCreateComputePipelines(m_context.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan compute pipeline");

        m_pipelines.emplace(shaderModule, pipeline);

        return pipeline;
    }

    void VulkanComputePipelines::destroy()
    {
        VkDevice device = m_context.device();

        for (const auto& [shaderModule, pipeline] : m_pipelines)
        {
            AIKO_UNUSED(shaderModule);

            if (pipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(device, pipeline, nullptr);
            }
        }

        m_pipelines.clear();

        if (m_layout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, m_layout, nullptr);
            m_layout = VK_NULL_HANDLE;
        }
    }
}
