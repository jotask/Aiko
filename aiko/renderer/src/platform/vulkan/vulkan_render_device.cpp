#include "vulkan_render_device.h"

#include <math/math_vector.h>
#include <logger/logger.h>
#include <intrumentor/profiler.h>

#include "vulkan_platform_helper.h"
#include "display/display_manager.h"
#include "impl/vulkan_computebuffer_impl.h"
#include "impl/vulkan_computeshader_impl.h"
#include "impl/vulkan_texture_impl.h"

#include <platform/vulkan/impl/vulkan_shader_impl.h>
#include <platform/vulkan/impl/vulkan_mesh_impl.h>
#include <platform/vulkan/impl/vulkan_framebuffer_impl.h>

#include "resources/render_resource_manager.h"

#include <array>
#include <cstring>

namespace aiko::renderer::vulkan
{

    VulkanRenderDevice::VulkanRenderDevice(RenderResourceManager* resources)
        : IRenderDevice(resources)
    {

    }

    VulkanRenderDevice::~VulkanRenderDevice()
    {
        AIKO_ASSERT(m_context.device() == VK_NULL_HANDLE, "VulkanRenderDevice destroyed without shutdown()");
    }

    bool VulkanRenderDevice::init(const DeviceInitDesc& desc)
    {

        const VkResult result = volkInitialize();
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to initialize Volk/Vulkan");

        logger::Log::info("Vulkan Initialized");

        m_context.init(desc);

        createComputePipelineLayout();
        createComputeDescriptorPool();

        createScreenPipelineLayout();
        createScreenPipeline();
        createScreenDescriptorPool();

        createModelPipelineLayout();
        createFrameResources();

        createMaterialResources();
        m_whiteTexture.create();
        m_whiteTexture.setPixels({WHITE});

        return true;
    }

    void VulkanRenderDevice::shutdown()
    {
        waitIdle();
        destroyMaterialResources();
        m_whiteTexture.unload();
        destroyFrameResources();
        destroyModelPipeline();
        destroyScreenPipeline();
        destroyTransientResources();
        destroyComputePipelineLayout();
        destroyComputePipeline();
        destroyComputeDescriptorPool();
        m_context.shutdown();
    }

    void VulkanRenderDevice::resize(u32 width, u32 height, bool vsync)
    {
        AIKO_UNUSED(width);
        AIKO_UNUSED(height);
        AIKO_UNUSED(vsync);
        m_context.requestSwapChainRecreation();
    }

    void VulkanRenderDevice::beginFrame()
    {
        m_frameActive = m_context.beginFrame();

        if (m_frameActive == false)
        {
            return;
        }

        if (m_context.consumeSwapChainFormatChanged() == true)
        {
            if (m_screenPipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(m_context.device(), m_screenPipeline, nullptr);
                m_screenPipeline = VK_NULL_HANDLE;
            }
            createScreenPipeline();
        }

    }

    void VulkanRenderDevice::endFrame()
    {

    }

    void VulkanRenderDevice::beginPass(uint16_t viewId, const PassDescription& pass, const FrameBuffer* frameBuffer)
    {
        if (!m_frameActive || viewId == COMPUTE_VIEW)
        {
            return;
        }

        VkRenderPass renderPass;
        VkFramebuffer framebuffer;
        VkExtent2D extent;

        if (frameBuffer != nullptr)
        {
            auto* fb = static_cast<VulkanFrameBufferImpl*>(frameBuffer->getImpl());
            renderPass = fb->renderPass();
            framebuffer = fb->framebuffer();
            extent = { fb->width(), fb->height() };
        }
        else
        {
            renderPass = m_context.renderPass();
            framebuffer = m_context.currentSwapChainFramebuffer();
            extent = m_context.swapChainExtent();
        }

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{ pass.clear.r, pass.clear.g, pass.clear.b, pass.clear.a }};
        clearValues[1].depthStencil = { 1.0f, 0 };

        m_activeRenderPass = renderPass;
        m_activeExtent = extent;

        if (viewId == SCENE_VIEW && m_modelPipelineTriangles == VK_NULL_HANDLE)
        {
            createModelPipeline(renderPass, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, m_modelPipelineTriangles);
        }

        const VkRenderPassBeginInfo renderPassInfo =
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass,
            .framebuffer = framebuffer,
            .renderArea =
            {
                .offset = { 0, 0 },
                .extent = extent,
            },
            .clearValueCount = static_cast<uint32_t>(clearValues.size()),
            .pClearValues = clearValues.data(),
        };

        vkCmdBeginRenderPass(m_context.activeCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        m_renderPassActive = true;

    }

    void VulkanRenderDevice::endPass()
    {
        if (m_renderPassActive == false)
        {
            return;
        }
        vkCmdEndRenderPass(m_context.activeCommandBuffer());
        m_renderPassActive = false;

        m_activeRenderPass = VK_NULL_HANDLE;
        m_activeExtent = {};

    }

    void VulkanRenderDevice::present()
    {
        if (m_frameActive == false)
        {
            return;
        }
        m_context.submitAndPresent();
        m_frameActive = false;
    }

    void VulkanRenderDevice::renderMesh(ViewId viewId, const mat4 world, const Mesh& mesh, const Material& material)
    {
        bindMaterial(material);
        drawMesh(viewId, world, mesh, material);
    }

    void VulkanRenderDevice::bindMaterial(const Material& material)
    {
        CachedMaterialBinding& binding = resolveMaterialBinding(material);

        VulkanMaterialUbo ubo{};
        ubo.u_baseColor = material.m_baseColor.toVec4();
        ubo.u_flags =
        {
            binding.hasTexture ? 1.0f : 0.0f,
            material.m_useVertexColor ? 1.0f : 0.0f,
            material.m_lit ? 1.0f : 0.0f,
            0.0f
        };

        auto setCustom = [&](const char* name, vec4& dst)
        {
            auto it = material.m_customVec4Uniforms.find(name);
            if (it != material.m_customVec4Uniforms.end())
            {
                dst = it->second;
            }
        };

        setCustom("u_particleSizeLife", ubo.u_particleSizeLife);
        setCustom("u_particleStartColor", ubo.u_particleStartColor);
        setCustom("u_particleEndColor", ubo.u_particleEndColor);
        setCustom("u_billboardParams", ubo.u_billboardParams);
        setCustom("u_nbodyRender", ubo.u_nbodyRender);

        std::memcpy(binding.uniformMapped, &ubo, sizeof(ubo));

        vkCmdBindDescriptorSets(m_context.activeCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_modelPipelineLayout, 1, 1, &binding.descriptorSet, 0, nullptr);
    }

    void VulkanRenderDevice::drawMesh(ViewId viewId, const mat4& world, const Mesh& mesh, const Material& material)
    {
        drawMeshWithPipeline(viewId, world, mesh, m_modelPipelineTriangles);
    }

    void VulkanRenderDevice::presentFrameBufferToScreen(ViewId viewId, const ScreenFbo& screen)
    {
        const Texture* texture = screen.getMaterial().m_runtimeDiffuseTexture;

        AIKO_ASSERT(texture != nullptr, "ScreenFbo has no runtime texture");
        AIKO_ASSERT(texture->isValid(), "ScreenFbo runtime texture is invalid");

        presentTextureToScreen(viewId, screen, *texture);
    }

    void VulkanRenderDevice::presentTextureToScreen(ViewId viewId, const ScreenFbo& screen, const Texture& texture)
    {
        AIKO_UNUSED(viewId);

        AIKO_ASSERT(m_renderPassActive, "Screen pass is not active");
        AIKO_ASSERT(m_screenPipeline != VK_NULL_HANDLE, "Screen pipeline is invalid");
        AIKO_ASSERT(m_screenPipelineLayout != VK_NULL_HANDLE, "Screen pipeline layout is invalid");

        VkDescriptorSet descriptorSet = getScreenDescriptorSet(texture);

        const Mesh& mesh = screen.getMesh();
        auto* meshImpl = static_cast<VulkanMeshImpl*>(mesh.getImpl());

        AIKO_ASSERT(meshImpl != nullptr, "Invalid screen mesh impl");
        AIKO_ASSERT(meshImpl->isValid(), "Invalid screen mesh");

        VkCommandBuffer commandBuffer = m_context.activeCommandBuffer();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_screenPipeline);

        const VkExtent2D extent = m_context.swapChainExtent();

        const VkViewport viewport =
        {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(extent.width),
            .height = static_cast<float>(extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        const VkRect2D scissor =
        {
            .offset = { 0, 0 },
            .extent = extent,
        };

        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_screenPipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

        const mat4 mvp = mat4(1.0f);

        vkCmdPushConstants(commandBuffer, m_screenPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mat4), &mvp);

        const VkBuffer vertexBuffers[] = { meshImpl->vertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, meshImpl->indexBuffer(), 0, VK_INDEX_TYPE_UINT16);

        vkCmdDrawIndexed(commandBuffer, meshImpl->indexCount(), 1, 0, 0, 0);
    }

    void VulkanRenderDevice::drawMeshInstanced(ViewId viewId, const Mesh& mesh, const Material& material, const void* data, u32 instanceCount, u32 instanceStrideBytes)
    {

    }

    void VulkanRenderDevice::bindFrame(ViewId viewId, const FrameData& u)
    {
        if (viewId != SCENE_VIEW)
        {
            return;
        }

        const uint32_t frame = m_context.currentFrameIndex();
        AIKO_ASSERT(frame < FramesInFlight, "Invalid Vulkan frame index");

        VulkanFrameUbo ubo{};

        ubo.u_view = u.view;
        ubo.u_projection = u.projection;
        ubo.u_viewProj = ubo.u_projection * ubo.u_view;
        m_sceneViewProj = ubo.u_viewProj;

        ubo.u_cameraPos = { u.cameraPosition.x, u.cameraPosition.y, u.cameraPosition.z, 1.0f };
        ubo.u_ambientColor = u.ambient.color.toVec4();
        ubo.u_ambientIntensity = { u.ambient.intensity, 0.0f, 0.0f, 0.0f };

        const uint32_t lightCount = static_cast<uint32_t>(u.lights.size() < MAX_LIGHTS ? u.lights.size() : MAX_LIGHTS);

        ubo.u_lightCount = { static_cast<float>(lightCount), 0.0f, 0.0f, 0.0f };

        for (uint32_t i = 0; i < lightCount; ++i)
        {
            const LightData& light = u.lights[i];

            ubo.u_lightType[i] =
            {
                static_cast<float>(static_cast<uint8_t>(light.type)),
                0.0f,
                0.0f,
                0.0f
            };

            ubo.u_lightPosRange[i] =
            {
                light.position.x,
                light.position.y,
                light.position.z,
                light.range
            };

            ubo.u_lightDir[i] =
            {
                light.direction.x,
                light.direction.y,
                light.direction.z,
                0.0f
            };

            ubo.u_lightColorInt[i] =
            {
                light.color.r,
                light.color.g,
                light.color.b,
                light.intensity
            };

            ubo.u_lightSpotCos[i] =
            {
                light.innerCos,
                light.outerCos,
                0.0f,
                0.0f
            };
        }

        std::memcpy(m_frameUniformMapped[frame], &ubo, sizeof(ubo));
        vkCmdBindDescriptorSets(m_context.activeCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_modelPipelineLayout, 0, 1, &m_frameDescriptorSets[frame], 0, nullptr );

    }

    void VulkanRenderDevice::execute(ViewId viewId, const ComputePass& pass)
    {
        AIKO_ASSERT(viewId == COMPUTE_VIEW, "Compute pass must use COMPUTE_VIEW");
        AIKO_ASSERT(pass.shader != nullptr, "Compute pass has no shader");
        AIKO_ASSERT(pass.shader->isValid(), "Invalid compute shader");
        AIKO_ASSERT(pass.buffers.size() == 1, "Vulkan compute currently supports exactly one buffer");
        AIKO_ASSERT(pass.images.empty(), "Vulkan compute images are not supported yet");
        AIKO_ASSERT(pass.vec4Uniforms.empty(), "Vulkan compute uniforms are not supported yet");

        const ComputeBufferBinding& binding = pass.buffers[0];

        AIKO_ASSERT(binding.buffer != nullptr, "Compute buffer binding is null");
        AIKO_ASSERT(binding.buffer->isValid(),"Invalid compute buffer");

        auto* shaderImpl = static_cast<VulkanComputeShaderImpl*>(pass.shader->getImpl());
        auto* bufferImpl = static_cast<VulkanComputeBufferImpl*>(binding.buffer->getImpl());

        AIKO_ASSERT(shaderImpl != nullptr, "Invalid Vulkan compute shader implementation");
        AIKO_ASSERT(bufferImpl != nullptr, "Invalid Vulkan compute buffer implementation");

        createComputePipeline(shaderImpl->module());

        updateComputeDescriptor(bufferImpl->buffer(), bufferImpl->size() );

        VkCommandBuffer commandBuffer = m_context.beginComputeCommands();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipelineLayout, 0, 1, &m_computeDescriptorSet, 0, nullptr);

        vkCmdDispatch(commandBuffer, pass.dispatch.groupsX, pass.dispatch.groupsY, pass.dispatch.groupsZ);

        m_context.endComputeCommands(commandBuffer);

    }

    void VulkanRenderDevice::requestReadback(const ComputeReadbackRequest& request)
    {

    }

    bool VulkanRenderDevice::pollReadback(ComputeReadbackResult& result)
    {
        return false;
    }

    void VulkanRenderDevice::drawMeshInstancedGpu(ViewId viewId, const GpuInstanceDrawDesc& desc)
    {

    }

    void VulkanRenderDevice::drawBillboards(ViewId viewId, const GpuBillboardDrawDesc& desc)
    {

    }

    void VulkanRenderDevice::drawTransient(ViewId viewId, const TransientDrawDesc& desc)
    {
        if (viewId != SCENE_VIEW || m_renderPassActive == false)
        {
            return;
        }

        AIKO_ASSERT(desc.material != nullptr, "Transient draw has no material");

        if (desc.geometry == nullptr || desc.geometry->vertices.empty())
        {
            return;
        }

        VkPipeline pipeline = VK_NULL_HANDLE;

        switch (desc.topology)
        {
            case TransientTopology::Triangles:
                pipeline = m_modelPipelineTriangles;
                break;

            case TransientTopology::Lines:
                if (m_transientLinePipeline == VK_NULL_HANDLE)
                {
                    createModelPipeline(m_activeRenderPass, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, m_transientLinePipeline);
                }
                pipeline = m_transientLinePipeline;
                break;

            case TransientTopology::Points:
                if (m_transientPointPipeline == VK_NULL_HANDLE)
                {
                    createModelPipeline(m_activeRenderPass, VK_PRIMITIVE_TOPOLOGY_POINT_LIST, m_transientPointPipeline);
                }
                pipeline = m_transientPointPipeline;
                break;
        }

        Mesh& mesh = resolveTransientMesh(*desc.geometry);
        drawMeshWithPipeline(viewId, desc.mtx, mesh, pipeline);

    }

    void VulkanRenderDevice::createFrameResources()
    {
        const VkDescriptorPoolSize poolSize =
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = static_cast<uint32_t>(FramesInFlight),
        };

        const VkDescriptorPoolCreateInfo poolInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = static_cast<uint32_t>(FramesInFlight),
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
        };

        const VkResult resultPoolCreation = vkCreateDescriptorPool(m_context.device(), &poolInfo, nullptr, &m_frameDescriptorPool);
        AIKO_ASSERT(resultPoolCreation == VK_SUCCESS, "Failed to create frame descriptor pool");

        const VkDeviceSize bufferSize = sizeof(VulkanFrameUbo);

        for (size_t i = 0; i < FramesInFlight; ++i)
        {
            m_context.createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                m_frameUniformBuffers[i],
                m_frameUniformMemories[i]
            );

            const VkResult resultMapMemory = vkMapMemory(m_context.device(), m_frameUniformMemories[i], 0, bufferSize, 0, &m_frameUniformMapped[i]);
            AIKO_ASSERT(resultMapMemory == VK_SUCCESS, "Failed to map frame uniform buffer");

        }

        std::array<VkDescriptorSetLayout, FramesInFlight> layouts{};
        layouts.fill(m_frameDescriptorSetLayout);

        const VkDescriptorSetAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_frameDescriptorPool,
            .descriptorSetCount = static_cast<uint32_t>(FramesInFlight),
            .pSetLayouts = layouts.data(),
        };

        const VkResult resultAllocDescSet = vkAllocateDescriptorSets(m_context.device(), &allocInfo, m_frameDescriptorSets.data() );
        AIKO_ASSERT(resultAllocDescSet == VK_SUCCESS, "Failed to allocate frame descriptor sets");

        for (size_t i = 0; i < FramesInFlight; ++i)
        {
            const VkDescriptorBufferInfo bufferInfo =
            {
                .buffer = m_frameUniformBuffers[i],
                .offset = 0,
                .range = sizeof(VulkanFrameUbo),
            };

            const VkWriteDescriptorSet write =
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_frameDescriptorSets[i],
                .dstBinding = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &bufferInfo,
            };

            vkUpdateDescriptorSets(m_context.device(), 1, &write, 0, nullptr);
        }
    }

    void VulkanRenderDevice::destroyFrameResources()
    {
        VkDevice device = m_context.device();

        for (size_t i = 0; i < FramesInFlight; ++i)
        {
            if (m_frameUniformMapped[i] != nullptr)
            {
                vkUnmapMemory(device, m_frameUniformMemories[i]);
            }

            if (m_frameUniformBuffers[i] != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(device, m_frameUniformBuffers[i], nullptr);
            }

            if (m_frameUniformMemories[i] != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, m_frameUniformMemories[i], nullptr);
            }

            m_frameUniformMapped[i] = nullptr;
            m_frameUniformBuffers[i] = VK_NULL_HANDLE;
            m_frameUniformMemories[i] = VK_NULL_HANDLE;
            m_frameDescriptorSets[i] = VK_NULL_HANDLE;
        }

        if (m_frameDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(device, m_frameDescriptorPool, nullptr);
        }

        m_frameDescriptorPool = VK_NULL_HANDLE;
    }

    void VulkanRenderDevice::createModelPipeline(VkRenderPass renderPass, VkPrimitiveTopology topology, VkPipeline& pipeline)
    {
        AIKO_ASSERT(renderPass != VK_NULL_HANDLE, "Model render pass is invalid");
        AIKO_ASSERT(m_modelPipelineLayout != VK_NULL_HANDLE, "Model pipeline layout is invalid");

        VulkanShaderImpl shader;
        shader.load("model.vs", "model.fs");

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
            .layout = m_modelPipelineLayout,
            .renderPass = renderPass,
            .subpass = 0,
        };

        const VkResult result = vkCreateGraphicsPipelines(m_context.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);

        shader.unload();

        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create model graphics pipeline");
    }

    void VulkanRenderDevice::createModelPipelineLayout()
    {
        const VkDescriptorSetLayoutBinding frameBinding =
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        };

        const VkDescriptorSetLayoutCreateInfo frameLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &frameBinding,
        };

        const VkResult resultDescriptionCreation = vkCreateDescriptorSetLayout(m_context.device(), &frameLayoutInfo, nullptr, &m_frameDescriptorSetLayout);
        AIKO_ASSERT(resultDescriptionCreation == VK_SUCCESS, "Failed to create frame descriptor set layout");

        const std::array<VkDescriptorSetLayoutBinding, 2> materialBindings =
        {
            VkDescriptorSetLayoutBinding
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            },
            VkDescriptorSetLayoutBinding
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            }
        };

        const VkDescriptorSetLayoutCreateInfo materialLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(materialBindings.size()),
            .pBindings = materialBindings.data(),
        };

        const VkResult resultCreationLayout = vkCreateDescriptorSetLayout(m_context.device(), &materialLayoutInfo, nullptr, &m_materialDescriptorSetLayout);
        AIKO_ASSERT(resultCreationLayout == VK_SUCCESS, "Failed to create material descriptor set layout");

        const std::array<VkDescriptorSetLayout, 2> setLayouts =
        {
            m_frameDescriptorSetLayout,
            m_materialDescriptorSetLayout
        };

        const VkPushConstantRange pushConstantRange =
        {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            .offset = 0,
            .size = sizeof(mat4) * 2,
        };

        const VkPipelineLayoutCreateInfo pipelineLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = static_cast<uint32_t>(setLayouts.size()),
            .pSetLayouts = setLayouts.data(),
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &pushConstantRange,
        };

        VkResult resultPipelineCreation = vkCreatePipelineLayout(m_context.device(), &pipelineLayoutInfo, nullptr, &m_modelPipelineLayout);
        AIKO_ASSERT(resultPipelineCreation == VK_SUCCESS, "Failed to create model pipeline layout");

    }

    void VulkanRenderDevice::destroyModelPipeline()
    {
        VkDevice device = m_context.device();

        if (m_modelPipelineTriangles != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(device, m_modelPipelineTriangles, nullptr);
            m_modelPipelineTriangles = VK_NULL_HANDLE;
        }

        if (m_transientPointPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(device, m_transientPointPipeline, nullptr);
            m_transientPointPipeline = VK_NULL_HANDLE;
        }

        if (m_transientLinePipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(device, m_transientLinePipeline, nullptr);
            m_transientLinePipeline = VK_NULL_HANDLE;
        }

        if (m_modelPipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, m_modelPipelineLayout, nullptr);
            m_modelPipelineLayout = VK_NULL_HANDLE;
        }

        if (m_materialDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, m_materialDescriptorSetLayout, nullptr);
            m_materialDescriptorSetLayout = VK_NULL_HANDLE;
        }

        if (m_frameDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, m_frameDescriptorSetLayout, nullptr);
            m_frameDescriptorSetLayout = VK_NULL_HANDLE;
        }

    }

    void VulkanRenderDevice::createScreenPipelineLayout()
    {
        const VkDescriptorSetLayoutBinding sceneBinding =
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        };

        const VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &sceneBinding,
        };

        const VkResult resultDescriptionSet = vkCreateDescriptorSetLayout(m_context.device(), &descriptorLayoutInfo, nullptr, &m_screenDescriptorSetLayout);
        AIKO_ASSERT(resultDescriptionSet == VK_SUCCESS, "Failed to create screen descriptor set layout");

        const VkPushConstantRange pushConstantRange =
        {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(mat4),
        };

        const VkPipelineLayoutCreateInfo pipelineLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 1,
            .pSetLayouts = &m_screenDescriptorSetLayout,
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &pushConstantRange,
        };

        const VkResult resultCreatePipeline = vkCreatePipelineLayout(m_context.device(), &pipelineLayoutInfo, nullptr, &m_screenPipelineLayout);
        AIKO_ASSERT(resultCreatePipeline == VK_SUCCESS, "Failed to create screen pipeline layout");

    }

    void VulkanRenderDevice::destroyScreenPipeline()
    {
        VkDevice device = m_context.device();

        if (m_screenDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(device, m_screenDescriptorPool, nullptr);
        }

        if (m_screenPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(device, m_screenPipeline, nullptr);
        }

        if (m_screenPipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, m_screenPipelineLayout, nullptr);
        }

        if (m_screenDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, m_screenDescriptorSetLayout, nullptr);
        }

        m_screenPipeline = VK_NULL_HANDLE;
        m_screenPipelineLayout = VK_NULL_HANDLE;
        m_screenDescriptorSetLayout = VK_NULL_HANDLE;

        m_screenDescriptorPool = VK_NULL_HANDLE;
        m_screenDescriptorSet = VK_NULL_HANDLE;
        m_screenDescriptorImageView = VK_NULL_HANDLE;
        m_screenDescriptorSampler = VK_NULL_HANDLE;

    }

    void VulkanRenderDevice::createScreenPipeline()
    {
        AIKO_ASSERT(m_screenPipelineLayout != VK_NULL_HANDLE, "Screen pipeline layout is invalid");

        VulkanShaderImpl shader;
        shader.load("passthrough.vs", "passthrough.fs");

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
            .layout = m_screenPipelineLayout,
            .renderPass = m_context.renderPass(),
            .subpass = 0,
        };

        const VkResult result = vkCreateGraphicsPipelines(
            m_context.device(),
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            nullptr,
            &m_screenPipeline
        );

        shader.unload();

        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create screen graphics pipeline");

    }

    void VulkanRenderDevice::createScreenDescriptorPool()
    {
        const VkDescriptorPoolSize poolSize =
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
        };

        const VkDescriptorPoolCreateInfo poolInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = 1,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
        };

        const VkResult result = vkCreateDescriptorPool(m_context.device(), &poolInfo, nullptr, &m_screenDescriptorPool);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create screen descriptor pool");

    }

    VkDescriptorSet VulkanRenderDevice::getScreenDescriptorSet(const Texture& texture)
    {
        AIKO_ASSERT(m_screenDescriptorPool != VK_NULL_HANDLE, "Screen descriptor pool is invalid");
        AIKO_ASSERT(m_screenDescriptorSetLayout != VK_NULL_HANDLE, "Screen descriptor set layout is invalid");

        auto* textureImpl = static_cast<VulkanTextureImpl*>(texture.getImpl());

        AIKO_ASSERT(textureImpl != nullptr, "Invalid screen texture impl");
        AIKO_ASSERT(textureImpl->isValid(), "Invalid screen texture");

        const VkImageView imageView = textureImpl->imageView();
        const VkSampler sampler = textureImpl->sampler();

        AIKO_ASSERT(imageView != VK_NULL_HANDLE, "Screen texture image view is invalid");
        AIKO_ASSERT(sampler != VK_NULL_HANDLE, "Screen texture sampler is invalid");

        if (m_screenDescriptorSet == VK_NULL_HANDLE)
        {
            const VkDescriptorSetAllocateInfo allocInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .descriptorPool = m_screenDescriptorPool,
                .descriptorSetCount = 1,
                .pSetLayouts = &m_screenDescriptorSetLayout,
            };

            const VkResult result = vkAllocateDescriptorSets(
                m_context.device(),
                &allocInfo,
                &m_screenDescriptorSet
            );

            AIKO_ASSERT(result == VK_SUCCESS, "Failed to allocate screen descriptor set");
        }

        if (m_screenDescriptorImageView != imageView ||
            m_screenDescriptorSampler != sampler)
        {
            const VkDescriptorImageInfo imageInfo =
            {
                .sampler = sampler,
                .imageView = imageView,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            };

            const VkWriteDescriptorSet write =
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = m_screenDescriptorSet,
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfo,
            };

            vkUpdateDescriptorSets(m_context.device(), 1, &write, 0, nullptr);

            m_screenDescriptorImageView = imageView;
            m_screenDescriptorSampler = sampler;
        }

        return m_screenDescriptorSet;
    }

    void VulkanRenderDevice::createMaterialResources()
    {
        const std::array<VkDescriptorPoolSize, 2> poolSizes =
        {
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = MaxMaterialBindings,
            },
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = MaxMaterialBindings,
            },
        };

        const VkDescriptorPoolCreateInfo poolInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = MaxMaterialBindings,
            .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
            .pPoolSizes = poolSizes.data(),
        };

        const VkResult result = vkCreateDescriptorPool(m_context.device(), &poolInfo, nullptr, &m_materialDescriptorPool);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create material descriptor pool");
    }

    void VulkanRenderDevice::destroyMaterialResources()
    {
        VkDevice device = m_context.device();

        for (auto& [material, binding] : m_materialBindingCache)
        {
            if (binding.uniformMapped != nullptr)
            {
                vkUnmapMemory(device, binding.uniformMemory);
            }

            if (binding.uniformBuffer != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(device, binding.uniformBuffer, nullptr);
            }

            if (binding.uniformMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(device, binding.uniformMemory, nullptr);
            }
        }

        m_materialBindingCache.clear();

        if (m_materialDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(device, m_materialDescriptorPool, nullptr);
            m_materialDescriptorPool = VK_NULL_HANDLE;
        }
    }

    VulkanRenderDevice::CachedMaterialBinding& VulkanRenderDevice::resolveMaterialBinding(const Material& material)
    {

        const MaterialBindingKey key = makeMaterialBindingKey(material);

        auto it = m_materialBindingCache.find(key);
        if (it != m_materialBindingCache.end())
        {
            return it->second;
        }

        CachedMaterialBinding binding{};

        m_context.createBuffer( sizeof(VulkanMaterialUbo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, binding.uniformBuffer, binding.uniformMemory);

        vkMapMemory(m_context.device(), binding.uniformMemory, 0, sizeof(VulkanMaterialUbo), 0, &binding.uniformMapped);

        const VkDescriptorSetAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_materialDescriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &m_materialDescriptorSetLayout,
        };

        VkResult result = vkAllocateDescriptorSets(m_context.device(), &allocInfo, &binding.descriptorSet);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to allocate material descriptor set");

        const Texture* texture = nullptr;
        bool hasRealTexture = false;

        if (material.m_runtimeDiffuseTexture != nullptr && material.m_runtimeDiffuseTexture->isValid())
        {
            texture = material.m_runtimeDiffuseTexture;
            hasRealTexture = true;
        }
        else if (material.m_diffuseTextureId != InvalidAssetId)
        {
            Texture& assetTexture = getResources()->getTexture(material.m_diffuseTextureId);
            if (assetTexture.isValid())
            {
                texture = &assetTexture;
                hasRealTexture = true;
            }
        }

        if (texture == nullptr)
        {
            texture = &m_whiteTexture;
        }

        auto* textureImpl = static_cast<VulkanTextureImpl*>(texture->getImpl());
        AIKO_ASSERT(textureImpl != nullptr && textureImpl->isValid(), "Invalid Vulkan material texture");

        binding.imageView = textureImpl->imageView();
        binding.sampler = textureImpl->sampler();
        binding.hasTexture = hasRealTexture;

        const VkDescriptorBufferInfo bufferInfo =
        {
            .buffer = binding.uniformBuffer,
            .offset = 0,
            .range = sizeof(VulkanMaterialUbo),
        };

        const VkDescriptorImageInfo imageInfo =
        {
            .sampler = binding.sampler,
            .imageView = binding.imageView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        const std::array<VkWriteDescriptorSet, 2> writes =
        {
            VkWriteDescriptorSet
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = binding.descriptorSet,
                .dstBinding = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &bufferInfo,
            },
            VkWriteDescriptorSet
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = binding.descriptorSet,
                .dstBinding = 1,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfo,
            }
        };

        vkUpdateDescriptorSets( m_context.device(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);

        auto [insertedIt, inserted] = m_materialBindingCache.emplace(key, binding);
        return insertedIt->second;

    }

    void VulkanRenderDevice::destroyTransientResources()
    {
        for (auto& [geometry, mesh] : m_transientMeshCache)
        {
            if (mesh != nullptr)
            {
                mesh->unload();
            }
        }

        m_transientMeshCache.clear();
    }

    Mesh& VulkanRenderDevice::resolveTransientMesh(const TransientGeometry& geometry)
    {
        if (auto it = m_transientMeshCache.find(&geometry); it != m_transientMeshCache.end())
        {
            return *it->second;
        }

        MeshAsset asset{};
        asset.m_vertices.reserve(geometry.vertices.size());
        asset.m_textCoord.reserve(geometry.vertices.size());
        asset.m_normals.reserve(geometry.vertices.size());
        asset.m_colors.reserve(geometry.vertices.size());

        for (const TransientVertex& v : geometry.vertices)
        {
            asset.m_vertices.push_back(v.position);
            asset.m_textCoord.push_back(v.uv);
            asset.m_normals.push_back(v.normal);
            asset.m_colors.push_back(v.color);
        }

        asset.m_indices = geometry.indices;

        auto mesh = std::make_unique<Mesh>();
        mesh->upload(asset);

        Mesh& ref = *mesh;
        m_transientMeshCache.emplace(&geometry, std::move(mesh));
        return ref;
    }

    void VulkanRenderDevice::drawMeshWithPipeline(ViewId viewId, const mat4& world, const Mesh& mesh, VkPipeline pipeline)
    {

        if (viewId != SCENE_VIEW || m_renderPassActive == false)
        {
            return;
        }

        AIKO_ASSERT(pipeline != VK_NULL_HANDLE, "Model pipeline is invalid");
        AIKO_ASSERT(m_modelPipelineLayout != VK_NULL_HANDLE, "Model pipeline layout is invalid");

        auto* meshImpl = static_cast<VulkanMeshImpl*>(mesh.getImpl());
        AIKO_ASSERT(meshImpl != nullptr, "Mesh has no Vulkan impl");
        AIKO_ASSERT(meshImpl->isValid(), "Invalid Vulkan mesh");

        VkCommandBuffer commandBuffer = m_context.activeCommandBuffer();

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        const VkViewport viewport =
        {
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>(m_activeExtent.width),
            .height = static_cast<float>(m_activeExtent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        };

        const VkRect2D scissor =
        {
            .offset = { 0, 0 },
            .extent = m_activeExtent,
        };

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        struct DrawPushConstants
        {
            mat4 u_model;
            mat4 u_modelViewProj;
        };

        const DrawPushConstants push =
        {
            .u_model = world,
            .u_modelViewProj = m_sceneViewProj * world,
        };

        vkCmdPushConstants(commandBuffer, m_modelPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(push), &push);

        const VkBuffer vertexBuffers[] = { meshImpl->vertexBuffer() };
        const VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, meshImpl->indexBuffer(), 0, VK_INDEX_TYPE_UINT16);

        vkCmdDrawIndexed(commandBuffer, meshImpl->indexCount(), 1, 0, 0, 0);
    }

    void VulkanRenderDevice::createComputePipelineLayout()
    {
        const VkDescriptorSetLayoutBinding bufferBinding =
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        };

        const VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &bufferBinding,
        };

        const VkResult descriptorResult = vkCreateDescriptorSetLayout(m_context.device(), &descriptorLayoutInfo, nullptr, &m_computeDescriptorSetLayout);

        AIKO_ASSERT(descriptorResult == VK_SUCCESS, "Failed to create compute descriptor set layout");

        const VkPipelineLayoutCreateInfo pipelineLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 1,
            .pSetLayouts = &m_computeDescriptorSetLayout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
        };

        const VkResult pipelineResult = vkCreatePipelineLayout(m_context.device(), &pipelineLayoutInfo, nullptr, &m_computePipelineLayout);

        AIKO_ASSERT(pipelineResult == VK_SUCCESS,"Failed to create compute pipeline layout");

    }

    void VulkanRenderDevice::destroyComputePipelineLayout()
    {
        VkDevice device = m_context.device();

        if (m_computePipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(device, m_computePipelineLayout, nullptr);
            m_computePipelineLayout = VK_NULL_HANDLE;
        }

        if (m_computeDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device,m_computeDescriptorSetLayout,nullptr);
            m_computeDescriptorSetLayout = VK_NULL_HANDLE;
        }
    }

    void VulkanRenderDevice::createComputePipeline(VkShaderModule shaderModule)
    {
        AIKO_ASSERT(shaderModule != VK_NULL_HANDLE, "Invalid compute shader module");

        if (m_computePipeline != VK_NULL_HANDLE && m_computePipelineShader == shaderModule)
        {
            return;
        }

        destroyComputePipeline();

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
            .layout = m_computePipelineLayout,
        };

        const VkResult result = vkCreateComputePipelines(m_context.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_computePipeline);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan compute pipeline");

        m_computePipelineShader = shaderModule;
    }

    void VulkanRenderDevice::destroyComputePipeline()
    {
        if (m_computePipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(m_context.device(), m_computePipeline, nullptr);
            m_computePipeline = VK_NULL_HANDLE;
        }

        m_computePipelineShader = VK_NULL_HANDLE;
    }

    void VulkanRenderDevice::createComputeDescriptorPool()
    {
        const VkDescriptorPoolSize poolSize =
        {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
        };

        const VkDescriptorPoolCreateInfo poolInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = 1,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
        };

        const VkResult poolResult = vkCreateDescriptorPool(m_context.device(), &poolInfo, nullptr, &m_computeDescriptorPool);
        AIKO_ASSERT(poolResult == VK_SUCCESS, "Failed to create compute descriptor pool");

        const VkDescriptorSetAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_computeDescriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &m_computeDescriptorSetLayout,
        };

        const VkResult allocResult = vkAllocateDescriptorSets(m_context.device(),&allocInfo,&m_computeDescriptorSet);
        AIKO_ASSERT(allocResult == VK_SUCCESS,"Failed to allocate compute descriptor set");
    }

    void VulkanRenderDevice::destroyComputeDescriptorPool()
    {
        if (m_computeDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool( m_context.device(), m_computeDescriptorPool,nullptr );
            m_computeDescriptorPool = VK_NULL_HANDLE;
            m_computeDescriptorSet = VK_NULL_HANDLE;
        }
    }

    void VulkanRenderDevice::updateComputeDescriptor(VkBuffer buffer, VkDeviceSize size)
    {
        AIKO_ASSERT(buffer != VK_NULL_HANDLE, "Invalid compute storage buffer");

        const VkDescriptorBufferInfo bufferInfo =
        {
            .buffer = buffer,
            .offset = 0,
            .range = size,
        };

        const VkWriteDescriptorSet write =
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = m_computeDescriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pBufferInfo = &bufferInfo,
        };

        vkUpdateDescriptorSets(m_context.device(), 1, &write, 0, nullptr);

    }

    void VulkanRenderDevice::waitIdle()
    {
        if (m_context.device() != VK_NULL_HANDLE)
        {
            vkDeviceWaitIdle(m_context.device());
        }
    }

    VulkanRenderDevice::MaterialBindingKey VulkanRenderDevice::makeMaterialBindingKey(const Material& material) const
    {
        return MaterialBindingKey
        {
            .shaderId = material.m_shaderId,
            .diffuseTextureId = material.m_diffuseTextureId,
            .runtimeDiffuseTexture = material.m_runtimeDiffuseTexture,
            .useVertexColor = material.m_useVertexColor,
            .lit = material.m_lit,
            .baseColor = material.m_baseColor.rgba(),
        };
    }

}
