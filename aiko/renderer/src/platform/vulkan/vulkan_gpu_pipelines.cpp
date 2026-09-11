#include "vulkan_gpu_pipelines.h"

#include <array>

#include "vulkan_context.h"
#include "vulkan_shader_reflector.h"
#include "vulkan_types.h"
#include "impl/vulkan_shader_impl.h"

namespace aiko::renderer::vulkan
{
    VulkanGpuPipelines::VulkanGpuPipelines(VulkanContext& context)
        : m_context(context)
    {
    }

    void VulkanGpuPipelines::create(VkPipelineLayout layout)
    {
        AIKO_ASSERT(m_layout == VK_NULL_HANDLE, "Vulkan GPU pipelines already created");
        AIKO_ASSERT(layout != VK_NULL_HANDLE, "GPU pipelines require a valid graphics pipeline layout");
        m_layout = layout;
    }

    VkPipeline VulkanGpuPipelines::getOrCreateInstanced(const GpuPipelineKey& key, VkRenderPass renderPass, const VulkanShaderImpl& shader)
    {
        AIKO_ASSERT(m_layout != VK_NULL_HANDLE, "GPU pipeline layout is invalid");
        AIKO_ASSERT(renderPass != VK_NULL_HANDLE, "GPU-instanced render pass is invalid");
        AIKO_ASSERT(shader.isValid(), "GPU-instanced shader is invalid");

        if (const auto it = m_instancedPipelines.find(key); it != m_instancedPipelines.end())
        {
            return it->second;
        }

        const VkPipeline pipeline = createInstancedPipeline(renderPass, shader);
        AIKO_ASSERT(pipeline != VK_NULL_HANDLE, "Failed to create GPU-instanced graphics pipeline");

        m_instancedPipelines.emplace(key, pipeline);

        return pipeline;
    }

    VkPipeline VulkanGpuPipelines::getOrCreateVertex(const GpuVertexPipelineKey& key, VkRenderPass renderPass, const VulkanShaderImpl& shader)
    {
        AIKO_ASSERT(m_layout != VK_NULL_HANDLE, "GPU pipeline layout is invalid");
        AIKO_ASSERT(renderPass != VK_NULL_HANDLE, "GPU vertex render pass is invalid");
        AIKO_ASSERT(shader.isValid(), "GPU vertex shader is invalid");

        if (const auto it = m_vertexPipelines.find(key); it != m_vertexPipelines.end())
        {
            return it->second;
        }

        const VkPipeline pipeline = createVertexPipeline(renderPass, key.topology, shader);
        AIKO_ASSERT(pipeline != VK_NULL_HANDLE, "Failed to create GPU vertex graphics pipeline");

        m_vertexPipelines.emplace(key, pipeline);

        return pipeline;
    }

    VkPipeline VulkanGpuPipelines::createInstancedPipeline(VkRenderPass renderPass, const VulkanShaderImpl& shader)
    {
        AIKO_ASSERT(renderPass != VK_NULL_HANDLE,"GPU-instanced render pass is invalid");
        AIKO_ASSERT(m_layout != VK_NULL_HANDLE, "GPU pipeline layout is invalid");

        validateModelShaderAbi(shader.reflection());
        validateModelPushConstants(shader.reflection());

        const VkPipelineShaderStageCreateInfo vertShaderStageInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = shader.vertexModule(),
            .pName = "main",
        };

        const VkPipelineShaderStageCreateInfo fragShaderStageInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = shader.fragmentModule(),
            .pName = "main",
        };

        const std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages =
        {
            vertShaderStageInfo,
            fragShaderStageInfo,
        };

        const VkVertexInputBindingDescription bindingDescription = VulkanVertex::bindingDescription();

        const auto attributeDescriptions = VulkanVertex::attributeDescriptions();

        const VkPipelineVertexInputStateCreateInfo vertexInputInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &bindingDescription,
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
            .pVertexAttributeDescriptions = attributeDescriptions.data(),
        };

        const VkPipelineInputAssemblyStateCreateInfo inputAssembly =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,
        };

        const VkPipelineViewportStateCreateInfo viewportState =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .scissorCount = 1,
        };

        const VkPipelineRasterizationStateCreateInfo rasterizer =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        const VkPipelineMultisampleStateCreateInfo multisampling =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
        };

        const VkPipelineDepthStencilStateCreateInfo depthStencil =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        const VkPipelineColorBlendAttachmentState colorBlendAttachment =
        {
            .blendEnable = VK_FALSE,
            .colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT,
        };

        const VkPipelineColorBlendStateCreateInfo  colorBlending =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment,
        };

        const std::array<VkDynamicState, 2> dynamicStates =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        const VkPipelineDynamicStateCreateInfo dynamicState =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data(),
        };

        const VkGraphicsPipelineCreateInfo pipelineInfo =
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = static_cast<uint32_t>(shaderStages.size()),
            .pStages = shaderStages.data(),
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = &depthStencil,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = m_layout,
            .renderPass = renderPass,
            .subpass = 0,
        };

        VkPipeline pipeline = VK_NULL_HANDLE;

        const VkResult result = vkCreateGraphicsPipelines(m_context.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create GPU-instanced graphics pipeline");

        return pipeline;
    }

    VkPipeline VulkanGpuPipelines::createVertexPipeline(VkRenderPass renderPass, VkPrimitiveTopology topology, const VulkanShaderImpl& shader)
    {
        AIKO_ASSERT(renderPass != VK_NULL_HANDLE, "GPU vertex render pass is invalid");
        AIKO_ASSERT(m_layout != VK_NULL_HANDLE, "GPU pipeline layout is invalid");

        validateModelShaderAbi(shader.reflection());
        validateModelPushConstants(shader.reflection());

        const VkPipelineShaderStageCreateInfo vertShaderStageInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = shader.vertexModule(),
            .pName = "main",
        };

        const VkPipelineShaderStageCreateInfo fragShaderStageInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = shader.fragmentModule(),
            .pName = "main",
        };

        const std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages =
        {
            vertShaderStageInfo,
            fragShaderStageInfo,
        };

        const VkVertexInputBindingDescription bindingDescription =
        {
            .binding = 0,
            .stride = sizeof(vec4),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        };

        const VkVertexInputAttributeDescription attributeDescription =
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = 0,
        };

        const VkPipelineVertexInputStateCreateInfo vertexInputInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &bindingDescription,
            .vertexAttributeDescriptionCount = 1,
            .pVertexAttributeDescriptions = &attributeDescription,
        };

        const VkPipelineInputAssemblyStateCreateInfo inputAssembly =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = topology,
            .primitiveRestartEnable = VK_FALSE,
        };

        const VkPipelineViewportStateCreateInfo viewportState =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .scissorCount = 1,
        };

        const VkPipelineRasterizationStateCreateInfo rasterizer =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        const VkPipelineMultisampleStateCreateInfo multisampling =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
        };

        const VkPipelineDepthStencilStateCreateInfo depthStencil =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        const VkPipelineColorBlendAttachmentState colorBlendAttachment =
        {
            .blendEnable = VK_FALSE,
            .colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT,
        };

        const VkPipelineColorBlendStateCreateInfo colorBlending =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment,
        };

        const std::array<VkDynamicState, 2> dynamicStates =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        const VkPipelineDynamicStateCreateInfo dynamicState =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data(),
        };

        const VkGraphicsPipelineCreateInfo pipelineInfo =
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = static_cast<uint32_t>(shaderStages.size()),
            .pStages = shaderStages.data(),
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = &depthStencil,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,
            .layout = m_layout,
            .renderPass = renderPass,
            .subpass = 0,
        };

        VkPipeline pipeline = VK_NULL_HANDLE;

        const VkResult result = vkCreateGraphicsPipelines(m_context.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create GPU vertex graphics pipeline");

        return pipeline;
    }

    void VulkanGpuPipelines::destroy()
    {
        VkDevice device = m_context.device();

        for (auto& [key, pipeline] : m_instancedPipelines)
        {
            AIKO_UNUSED(key);

            if (pipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(device, pipeline, nullptr);
            }
        }

        m_instancedPipelines.clear();

        for (auto& [key, pipeline] : m_vertexPipelines)
        {
            AIKO_UNUSED(key);
            if (pipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(device, pipeline, nullptr);
            }
        }

        m_vertexPipelines.clear();

        // Borrowed from VulkanModelPipelines.
        // Do not destroy the VkPipelineLayout here.
        m_layout = VK_NULL_HANDLE;
    }
}
