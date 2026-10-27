#include "vulkan_model_pipelines.h"

#include "impl/vulkan_shader_impl.h"
#include "resource_result.h"
#include "vulkan_context.h"
#include "vulkan_descriptor_abi.h"
#include "vulkan_render_state_utils.h"
#include "vulkan_shader_reflector.h"
#include "vulkan_types.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <filesystem>

namespace aiko::renderer::vulkan
{

    VulkanModelPipelines::VulkanModelPipelines(VulkanContext& context)
        : m_context(context)
    {
    }

    void VulkanModelPipelines::create(VkDescriptorSetLayout frameLayout, VkDescriptorSetLayout gpuReadLayout)
    {
        AIKO_ASSERT(m_layout == VK_NULL_HANDLE, "Vulkan model pipelines already created");
        AIKO_ASSERT(m_materialLayout == VK_NULL_HANDLE, "Vulkan model material layout already created");

        createMaterialLayout();
        createShadowLayout();
        createPipelineLayout(frameLayout, gpuReadLayout);
    }

    void VulkanModelPipelines::createMaterialLayout()
    {
        const std::array<VkDescriptorSetLayoutBinding, 1 + abi::MaxMaterialTextureBindings> materialBindings = []
        {
            std::array<VkDescriptorSetLayoutBinding, 1 + abi::MaxMaterialTextureBindings> result{};

            result[0] =
            {
                .binding = abi::MaterialUboBinding,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            };

            for (uint32_t i = 0; i < abi::MaxMaterialTextureBindings;++i)
            {
                result[i + 1] =
                {
                    .binding = abi::MaterialTextureBindingBase + i,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    .pImmutableSamplers = nullptr,
                };
            }

            return result;
        }();

        const VkDescriptorSetLayoutCreateInfo layoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(materialBindings.size()),
            .pBindings = materialBindings.data(),
        };

        const VkResult result = vkCreateDescriptorSetLayout(m_context.device(), &layoutInfo, nullptr, &m_materialLayout);

        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create material descriptor set layout");
    }

    void VulkanModelPipelines::createPipelineLayout(VkDescriptorSetLayout frameLayout, VkDescriptorSetLayout gpuReadLayout)
    {
        AIKO_ASSERT(frameLayout != VK_NULL_HANDLE, "Frame descriptor layout is invalid");
        AIKO_ASSERT(m_materialLayout != VK_NULL_HANDLE, "Material descriptor layout is invalid");
        AIKO_ASSERT(gpuReadLayout != VK_NULL_HANDLE, "GPU-read descriptor layout is invalid");
        AIKO_ASSERT(m_shadowLayout != VK_NULL_HANDLE, "Shadow descriptor layout is invalid");

        std::array<VkDescriptorSetLayout, 4> setLayouts{};

        setLayouts[abi::GraphicsFrameSet] = frameLayout;
        setLayouts[abi::GraphicsMaterialSet] = m_materialLayout;
        setLayouts[abi::GraphicsGpuReadSet] = gpuReadLayout;
        setLayouts[abi::GraphicsShadowSet] = m_shadowLayout;

        const VkPushConstantRange pushConstantRange =
        {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            .offset = 0,
            .size = sizeof(mat4) * 2,
        };

        const VkPipelineLayoutCreateInfo info =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = static_cast<uint32_t>(setLayouts.size()),
            .pSetLayouts = setLayouts.data(),
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &pushConstantRange,
        };

        const VkResult result = vkCreatePipelineLayout(m_context.device(), &info, nullptr, &m_layout);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create model pipeline layout");
    }

    VkPipeline VulkanModelPipelines::getOrCreate(const ModelPipelineKey& key, VkRenderPass renderPass, const VulkanShaderImpl& shader)
    {
        AIKO_ASSERT(key.instanced == false, "Normal model pipeline cannot use instanced key");
        if (const auto it = m_pipelines.find(key); it != m_pipelines.end())
        {
            return it->second;
        }
        const VkPipeline pipeline = createModelPipeline(key, renderPass, shader);
        AIKO_ASSERT(pipeline != VK_NULL_HANDLE, "Failed to create Vulkan model pipeline");
        m_pipelines.emplace(key,pipeline);
        return pipeline;
    }

    VkPipeline VulkanModelPipelines::getOrCreateInstanced(const ModelPipelineKey& key, VkRenderPass renderPass, const IResourceResolver& resources)
    {
        AIKO_ASSERT(key.instanced, "Instanced model pipeline requires instanced key");
        if (const auto it = m_pipelines.find(key); it != m_pipelines.end())
        {
            return it->second;
        }
        const VkPipeline pipeline = createInstancedPipeline(key, renderPass, resources);
        AIKO_ASSERT(pipeline != VK_NULL_HANDLE, "Failed to create Vulkan model instanced pipeline");
        m_pipelines.emplace(key, pipeline);
        return pipeline;
    }

    VkPipeline VulkanModelPipelines::createModelPipeline(const ModelPipelineKey& key, VkRenderPass renderPass, const VulkanShaderImpl& shader)
    {
        AIKO_ASSERT(renderPass != VK_NULL_HANDLE, "Model render pass is invalid");
        AIKO_ASSERT(m_layout != VK_NULL_HANDLE, "Model pipeline layout is invalid");
        AIKO_ASSERT(key.shaderId != InvalidAssetId, "Model material has invalid shader id");
        AIKO_ASSERT(shader.isValid(), "Invalid Vulkan material shader");

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
            fragShaderStageInfo
        };

        const VkVertexInputBindingDescription bindingDescription = VulkanVertex::bindingDescription();

        const auto vertexAttributes = VulkanVertex::attributeDescriptions();

        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.reserve(vertexAttributes.size());

        for (const uint32_t location : shader.reflection().vertexInputLocations)
        {
            const auto it =
                std::find_if(
                    vertexAttributes.begin(),
                    vertexAttributes.end(),
                    [location](const VkVertexInputAttributeDescription& attribute)
                    {
                        return attribute.location == location;
                    });

            AIKO_ASSERT(it != vertexAttributes.end(), "Model vertex shader uses unsupported vertex input location");

            attributeDescriptions.push_back(*it);
        }

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
            .topology = key.topology,
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
            .polygonMode = toVulkanPolygonMode(key.fillMode, m_context.capabilities()),
            .cullMode = toVulkanCullMode(key.cullMode),
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
            .depthTestEnable = key.depthTest ? VK_TRUE : VK_FALSE,
            .depthWriteEnable = key.depthWrite ? VK_TRUE : VK_FALSE,
            .depthCompareOp = toVulkanDepthCompare(key.depthCompare),
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        const VkPipelineColorBlendAttachmentState colorBlendAttachment =
        {
            .blendEnable = key.blend ? VK_TRUE : VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT,
        };

        const bool hasColorAttachment = key.renderPass.colorFormat != VK_FORMAT_UNDEFINED;

        const VkPipelineColorBlendStateCreateInfo colorBlending =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .attachmentCount = hasColorAttachment ? 1u : 0u,
            .pAttachments = hasColorAttachment ? &colorBlendAttachment : nullptr,
        };

        const std::array<VkDynamicState, 3> dynamicStates =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_LINE_WIDTH,
        };

        const VkPipelineDynamicStateCreateInfo dynamicState =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data(),
        };

        const VkGraphicsPipelineCreateInfo  pipelineInfo =
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
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create model graphics pipeline");

        return pipeline;
    }

    VkPipeline VulkanModelPipelines::createInstancedPipeline(const ModelPipelineKey& key, VkRenderPass renderPass, const IResourceResolver& resources)
    {
        AIKO_ASSERT(renderPass != VK_NULL_HANDLE, "Model instanced render pass is invalid");
        AIKO_ASSERT(m_layout != VK_NULL_HANDLE, "Model pipeline layout is invalid");

        VulkanShaderImpl shader;

        const ResourceResult vertex = resources.resolve("renderer://model_instanced.vert.spv");
        const ResourceResult fragment = resources.resolve("renderer://model_instanced.frag.spv");

        AIKO_ASSERTF(vertex && fragment, "Failed to resolve instanced model shader: vertex='%s', fragment='%s'", vertex.message.c_str(), fragment.message.c_str());

        const std::string vertexPath = vertex.path.string();
        const std::string fragmentPath = fragment.path.string();

        shader.load(vertexPath.c_str(), fragmentPath.c_str());

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
            fragShaderStageInfo
        };

        const VkVertexInputBindingDescription meshBinding = VulkanVertex::bindingDescription();

        const VkVertexInputBindingDescription instanceBinding =
        {
            .binding = 1,
            .stride = sizeof(VulkanInstanceData),
            .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
        };

        const std::array<VkVertexInputBindingDescription, 2> bindings =
        {
            meshBinding,
            instanceBinding
        };

        const auto meshAttributes = VulkanVertex::attributeDescriptions();

        std::vector<VkVertexInputAttributeDescription> supportedAttributes;
        supportedAttributes.reserve(9);

        supportedAttributes.insert(
            supportedAttributes.end(),
            meshAttributes.begin(),
            meshAttributes.end());

        supportedAttributes.push_back(
            {
                .location = 4,
                .binding = 1,
                .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                .offset = offsetof(VulkanInstanceData, position),
            });

        supportedAttributes.push_back(
            {
                .location = 5,
                .binding = 1,
                .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                .offset = offsetof(VulkanInstanceData, rotation),
            });

        supportedAttributes.push_back(
            {
                .location = 6,
                .binding = 1,
                .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                .offset = offsetof(VulkanInstanceData, scale),
            });

        supportedAttributes.push_back(
            {
                .location = 7,
                .binding = 1,
                .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                .offset = offsetof(VulkanInstanceData, color),
            });

        std::vector<VkVertexInputAttributeDescription> attributes;
        attributes.reserve(supportedAttributes.size());

        for (const uint32_t location : shader.reflection().vertexInputLocations)
        {
            const auto it =
                std::find_if(
                    supportedAttributes.begin(),
                    supportedAttributes.end(),
                    [location](const VkVertexInputAttributeDescription& attribute)
                    {
                        return attribute.location == location;
                    });

            AIKO_ASSERT(it != supportedAttributes.end(), "Instanced model vertex shader uses unsupported vertex input location");

            attributes.push_back(*it);
        }

        const VkPipelineVertexInputStateCreateInfo  vertexInputInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size()),
            .pVertexBindingDescriptions = bindings.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size()),
            .pVertexAttributeDescriptions = attributes.data(),
        };

        const VkPipelineInputAssemblyStateCreateInfo  inputAssembly =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,
        };

        const VkPipelineViewportStateCreateInfo  viewportState =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .scissorCount = 1,
        };

        const VkPipelineRasterizationStateCreateInfo  rasterizer =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = toVulkanPolygonMode(key.fillMode, m_context.capabilities()),
            .cullMode = toVulkanCullMode(key.cullMode),
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        const VkPipelineMultisampleStateCreateInfo  multisampling =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
        };

        const VkPipelineDepthStencilStateCreateInfo  depthStencil =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = key.depthTest ? VK_TRUE : VK_FALSE,
            .depthWriteEnable = key.depthWrite ? VK_TRUE : VK_FALSE,
            .depthCompareOp = toVulkanDepthCompare(key.depthCompare),
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        const VkPipelineColorBlendAttachmentState colorBlendAttachment =
        {
            .blendEnable = key.blend ? VK_TRUE : VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT,
        };

        const bool hasColorAttachment = key.renderPass.colorFormat != VK_FORMAT_UNDEFINED;

        const VkPipelineColorBlendStateCreateInfo colorBlending =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .attachmentCount = hasColorAttachment ? 1u : 0u,
            .pAttachments = hasColorAttachment ? &colorBlendAttachment : nullptr,
        };

        const std::array<VkDynamicState, 3> dynamicStates =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_LINE_WIDTH,
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
        shader.unload();

        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create model instanced graphics pipeline");

        return pipeline;
    }

    void VulkanModelPipelines::createShadowLayout()
    {
        AIKO_ASSERT(m_shadowLayout == VK_NULL_HANDLE, "Shadow descriptor layout already created");

        const std::array<VkDescriptorSetLayoutBinding, 2> bindings{{
            {
                .binding = abi::ShadowUboBinding,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            },
            {
                .binding = abi::ShadowTextureBinding,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            }
        }};

        const VkDescriptorSetLayoutCreateInfo info{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data(),
        };

        const VkResult result = vkCreateDescriptorSetLayout(m_context.device(), &info, nullptr, &m_shadowLayout);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create shadow descriptor layout");
    }

    void VulkanModelPipelines::destroy()
    {
        VkDevice device = m_context.device();

        for (auto& [key, pipeline] : m_pipelines)
        {
            AIKO_UNUSED(key);
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

        if (m_materialLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, m_materialLayout, nullptr);
            m_materialLayout = VK_NULL_HANDLE;
        }

        if (m_shadowLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, m_shadowLayout, nullptr);
            m_shadowLayout = VK_NULL_HANDLE;
        }
    }
}
