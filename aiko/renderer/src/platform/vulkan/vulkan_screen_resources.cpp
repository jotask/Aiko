#include "vulkan_screen_resources.h"

#include <algorithm>
#include <array>

#include "vulkan_context.h"
#include "vulkan_shader_reflector.h"
#include "vulkan_types.h"
#include "impl/vulkan_shader_impl.h"

namespace aiko::renderer::vulkan
{
    VulkanScreenResources::VulkanScreenResources(VulkanContext& context, uint32_t frameCount)
        : m_context(context)
        , m_descriptorSets(frameCount, VK_NULL_HANDLE)
    {
        AIKO_ASSERT(frameCount > 0, "Vulkan screen resources require at least one frame");
    }

    void VulkanScreenResources::create()
    {
        AIKO_ASSERT(m_descriptorSetLayout == VK_NULL_HANDLE, "Vulkan screen resources already created");
        createDescriptorResources();
        createPipelineLayout();
        createPipeline();
    }

    void VulkanScreenResources::createDescriptorResources()
    {
        const VkDescriptorSetLayoutBinding sceneBinding =
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        };

        const VkDescriptorSetLayoutCreateInfo layoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &sceneBinding,
        };

        const VkResult layoutResult = vkCreateDescriptorSetLayout(m_context.device(), &layoutInfo, nullptr, &m_descriptorSetLayout);
        AIKO_ASSERT(layoutResult == VK_SUCCESS, "Failed to create screen descriptor set layout");

        const VkDescriptorPoolSize poolSize =
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = static_cast<uint32_t>(m_descriptorSets.size()),
        };

        const VkDescriptorPoolCreateInfo poolInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = static_cast<uint32_t>(m_descriptorSets.size()),
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
        };

        const VkResult poolResult = vkCreateDescriptorPool(m_context.device(), &poolInfo, nullptr, &m_descriptorPool);
        AIKO_ASSERT(poolResult == VK_SUCCESS, "Failed to create screen descriptor pool");

        std::vector<VkDescriptorSetLayout> layouts( m_descriptorSets.size(), m_descriptorSetLayout);

        const VkDescriptorSetAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_descriptorPool,
            .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
            .pSetLayouts = layouts.data(),
        };

        const VkResult allocResult = vkAllocateDescriptorSets(m_context.device(),&allocInfo,m_descriptorSets.data());

        AIKO_ASSERT(allocResult == VK_SUCCESS, "Failed to allocate screen descriptor sets");
    }

    void VulkanScreenResources::createPipelineLayout()
    {
        AIKO_ASSERT(m_descriptorSetLayout != VK_NULL_HANDLE, "Screen descriptor layout is invalid");

        const VkPushConstantRange pushConstantRange =
        {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(mat4),
        };

        const VkPipelineLayoutCreateInfo info =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 1,
            .pSetLayouts = &m_descriptorSetLayout,
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &pushConstantRange,
        };

        const VkResult result = vkCreatePipelineLayout(m_context.device(),&info,nullptr,&m_pipelineLayout);

        AIKO_ASSERT(result == VK_SUCCESS,"Failed to create screen pipeline layout");
    }

    void VulkanScreenResources::createPipeline()
    {
        AIKO_ASSERT(m_pipelineLayout != VK_NULL_HANDLE, "Screen pipeline layout is invalid");

        VulkanShaderImpl shader;
        shader.load("passthrough.vs", "passthrough.fs");

        validateScreenShaderAbi(shader.reflection());
        validateScreenPushConstants(shader.reflection());

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
            fragShaderStageInfo
        };

        const VkVertexInputBindingDescription bindingDescription = VulkanVertex::bindingDescription();

        const auto vertexAttributes = VulkanVertex::attributeDescriptions();

        const std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions =
            {
                vertexAttributes[0],
                vertexAttributes[2],
            };

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
            .depthTestEnable = VK_FALSE,
            .depthWriteEnable = VK_FALSE,
            .depthCompareOp = VK_COMPARE_OP_ALWAYS,
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
            .layout = m_pipelineLayout,
            .renderPass = m_context.renderPass(),
            .subpass = 0,
        };

        const VkResult result = vkCreateGraphicsPipelines(m_context.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline);
        shader.unload();

        AIKO_ASSERT(result == VK_SUCCESS,"Failed to create screen graphics pipeline");
    }

    void VulkanScreenResources::destroyPipeline()
    {
        if (m_pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(m_context.device(), m_pipeline, nullptr);
            m_pipeline = VK_NULL_HANDLE;
        }
    }

    void VulkanScreenResources::recreatePipeline()
    {
        AIKO_ASSERT(m_pipelineLayout != VK_NULL_HANDLE, "Screen pipeline layout is invalid");
        destroyPipeline();
        createPipeline();
    }

    VkDescriptorSet VulkanScreenResources::descriptorSet(uint32_t frameIndex, VkImageView imageView, VkSampler sampler)
    {
        AIKO_ASSERT(frameIndex < m_descriptorSets.size(),"Invalid screen descriptor frame index");
        AIKO_ASSERT(imageView != VK_NULL_HANDLE, "Screen texture image view is invalid");
        AIKO_ASSERT(sampler != VK_NULL_HANDLE, "Screen texture sampler is invalid");

        const VkDescriptorSet descriptorSet = m_descriptorSets[frameIndex];
        AIKO_ASSERT(descriptorSet != VK_NULL_HANDLE, "Screen descriptor set is invalid");

        const VkDescriptorImageInfo imageInfo =
        {
            .sampler = sampler,
            .imageView = imageView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        const VkWriteDescriptorSet write =
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &imageInfo,
        };

        vkUpdateDescriptorSets(m_context.device(),1,&write,0,nullptr);

        return descriptorSet;
    }

    void VulkanScreenResources::destroy()
    {
        VkDevice device = m_context.device();

        destroyPipeline();

        if (m_pipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
            m_pipelineLayout = VK_NULL_HANDLE;
        }

        if (m_descriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
            m_descriptorPool = VK_NULL_HANDLE;
        }

        if (m_descriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, m_descriptorSetLayout, nullptr);
            m_descriptorSetLayout = VK_NULL_HANDLE;
        }

        std::fill(m_descriptorSets.begin(), m_descriptorSets.end(),VK_NULL_HANDLE);
    }
}
