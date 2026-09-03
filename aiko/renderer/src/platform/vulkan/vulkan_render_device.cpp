#include "vulkan_render_device.h"

#include <math/math_vector.h>
#include <logger/logger.h>
#include <intrumentor/profiler.h>

#include "vulkan_platform_helper.h"
#include "vulkan_shader_reflector.h"
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
#include <limits>

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
        createComputeDescriptorPools();

        createScreenPipelineLayout();
        createScreenPipeline();
        createScreenDescriptorPool();

        createGpuReadResources();
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
        destroyGpuInstancedPipelines();
        destroyGpuVertexPipelines();
        destroyModelPipeline();
        destroyGpuReadResources();
        destroyScreenPipeline();
        destroyTransientResources();
        destroyComputePipelines();
        destroyComputeDescriptorPools();
        destroyComputePipelineLayout();
        destroyReadbackResources();
        destroyUploadArena();
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

        const uint32_t frame = m_context.currentFrameIndex();
        AIKO_ASSERT(frame < FramesInFlight, "Invalid Vulkan frame index");

        completeReadbacksForFrame(frame);
        resetUploadArenaForFrame(frame);

        const VkResult resetResult = vkResetDescriptorPool(m_context.device(), m_computeDescriptorPools[frame], 0);
        AIKO_ASSERT(resetResult == VK_SUCCESS, "Failed to reset compute descriptor pool");

        if (m_context.consumeSwapChainFormatChanged() == true)
        {
            if (m_screenPipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(m_context.device(), m_screenPipeline, nullptr);
                m_screenPipeline = VK_NULL_HANDLE;
            }
            createScreenPipeline();
        }

        const VkResult gpuReadReset = vkResetDescriptorPool(m_context.device(), m_gpuReadDescriptorPools[frame], 0);
        AIKO_ASSERT(gpuReadReset == VK_SUCCESS, "Failed to reset GPU-read descriptor pool");

    }

    void VulkanRenderDevice::endFrame()
    {
        if (m_frameActive == false)
        {
            return;
        }
        AIKO_ASSERT(m_renderPassActive == false, "Cannot record readback while render pass is active");
        AIKO_ASSERT(m_computePassActive == false, "Cannot end frame while compute pass is active");
        recordReadbackCopies();
    }

    void VulkanRenderDevice::beginPass(uint16_t viewId, const PassDescription& pass, const FrameBuffer* frameBuffer)
    {
        if (m_frameActive == false)
        {
            return;
        }

        if (viewId == COMPUTE_VIEW)
        {
            AIKO_ASSERT(m_renderPassActive == false, "Compute pass cannot begin inside a graphics render pass");
            AIKO_ASSERT(m_computePassActive == false, "Compute pass is already active");
            m_computePassActive = true;
            return;
        }

        AIKO_ASSERT(m_computePassActive == false, "Graphics pass cannot begin while compute pass is active");

        VkRenderPass renderPass;
        VkFramebuffer framebuffer;
        VkExtent2D extent;

        if (frameBuffer != nullptr)
        {
            auto* fb = static_cast<VulkanFrameBufferImpl*>(frameBuffer->getImpl());
            renderPass = fb->renderPass();
            framebuffer = fb->framebuffer();
            extent = { fb->width(), fb->height() };

            auto* colorImpl = static_cast<VulkanTextureImpl*>(frameBuffer->getColorTexture().getImpl());
            AIKO_ASSERT(colorImpl != nullptr, "Invalid Vulkan framebuffer color texture");

            const VulkanImageState colorState = colorImpl->state();

            if (colorState.queueFamily == VK_QUEUE_FAMILY_IGNORED)
            {
                colorImpl->setState(
                {
                    .layout = colorState.layout,
                    .stage = colorState.stage,
                    .access = colorState.access,
                    .queueFamily = m_context.graphicsQueueFamily(),
                });
            }
            else if (colorState.queueFamily == m_context.computeQueueFamily() && m_context.hasDedicatedComputeQueue())
            {
                const VulkanImageState graphicsOwnershipState =
                {
                    .layout = colorState.layout,
                    .stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    .queueFamily = m_context.graphicsQueueFamily(),
                };

                VkCommandBuffer computeCommandBuffer = m_context.computeCommandBuffer();
                VkCommandBuffer graphicsCommandBuffer = m_context.activeCommandBuffer();

                releaseTextureOwnership(computeCommandBuffer, *colorImpl, graphicsOwnershipState);
                acquireTextureOwnership(graphicsCommandBuffer, *colorImpl, colorState, graphicsOwnershipState);
            }
            else
            {
                AIKO_ASSERT(colorState.queueFamily == m_context.graphicsQueueFamily(), "Framebuffer color texture has unexpected queue ownership");
            }

            m_activeColorAttachment = colorImpl;

        }
        else
        {
            m_activeColorAttachment = nullptr;
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
        if (m_computePassActive)
        {
            m_computePassActive = false;
            return;
        }

        if (m_renderPassActive == false)
        {
            return;
        }

        vkCmdEndRenderPass(m_context.activeCommandBuffer());

        if (m_activeColorAttachment != nullptr)
        {
            m_activeColorAttachment->setState(
            {
                .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                .stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                .access = VK_ACCESS_SHADER_READ_BIT,
                .queueFamily = m_context.graphicsQueueFamily(),
            });

            m_activeColorAttachment = nullptr;
        }

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

        vkCmdBindDescriptorSets(m_context.activeCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_modelPipelineLayout, abi::GraphicsMaterialSet, 1, &binding.descriptorSet, 0, nullptr);
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
        if (viewId != SCENE_VIEW || m_renderPassActive == false)
        {
            return;
        }

        AIKO_ASSERT(data != nullptr, "Instanced draw has no instance data");
        AIKO_ASSERT(instanceCount > 0, "Instanced draw has zero instances");
        AIKO_ASSERT(instanceStrideBytes >= sizeof(InstanceData), "Instanced draw stride is smaller than InstanceData");

        auto* meshImpl = static_cast<VulkanMeshImpl*>(mesh.getImpl());

        AIKO_ASSERT(meshImpl != nullptr, "Instanced mesh has no Vulkan implementation");
        AIKO_ASSERT(meshImpl->isValid(), "Invalid Vulkan instanced mesh");

        if (m_modelInstancedPipeline == VK_NULL_HANDLE)
        {
            createModelInstancedPipeline(m_activeRenderPass);
        }

        const VkDeviceSize instanceBytes = sizeof(VulkanInstanceData) * static_cast<VkDeviceSize>(instanceCount);

        const UploadSlice slice = allocateUploadSlice(m_context.currentFrameIndex(), instanceBytes, 16);

        const auto* source = static_cast<const uint8_t*>(data);
        auto* destination = static_cast<VulkanInstanceData*>(slice.mapped);

        for (u32 i = 0; i < instanceCount; ++i)
        {
            InstanceData instance{};
            std::memcpy(&instance, source + static_cast<size_t>(i) * instanceStrideBytes, sizeof(InstanceData));

            destination[i] =
            {
                .position =
                {
                    instance.position.x,
                    instance.position.y,
                    instance.position.z,
                    0.0f
                },
                .rotation =
                {
                    instance.rotation.x,
                    instance.rotation.y,
                    instance.rotation.z,
                    0.0f
                },
                .scale =
                {
                    instance.scale.x,
                    instance.scale.y,
                    instance.scale.z,
                    0.0f
                },
                .color = instance.color.toVec4(),
            };
        }

        bindMaterial(material);

        VkCommandBuffer commandBuffer = m_context.activeCommandBuffer();
        AIKO_ASSERT(commandBuffer != VK_NULL_HANDLE, "Instanced draw requires an active command buffer");

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_modelInstancedPipeline);

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

        const mat4 world = mat4(1.0f);

        const DrawPushConstants push =
        {
            .u_model = world,
            .u_modelViewProj = m_sceneViewProj * world,
        };

        vkCmdPushConstants(commandBuffer, m_modelPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(push), &push);

        const VkBuffer vertexBuffers[] =
        {
            meshImpl->vertexBuffer(),
            slice.buffer
        };

        const VkDeviceSize offsets[] =
        {
            0,
            slice.offset
        };

        vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offsets);
        vkCmdBindIndexBuffer( commandBuffer, meshImpl->indexBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed( commandBuffer, meshImpl->indexCount(), instanceCount, 0, 0, 0);
    }

    void VulkanRenderDevice::bindFrame(ViewId viewId, const FrameData& u)
    {
        if (viewId != SCENE_VIEW && viewId != COMPUTE_VIEW)
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

        ubo.u_time = { u.time, u.deltaTime, 0.0f, 0.0f };

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
        if (viewId == SCENE_VIEW)
        {
            vkCmdBindDescriptorSets(m_context.activeCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_modelPipelineLayout, abi::GraphicsFrameSet, 1, &m_frameDescriptorSets[frame], 0, nullptr );
        }

    }

    void VulkanRenderDevice::execute(ViewId viewId, const ComputePass& pass)
    {

        AIKO_ASSERT(viewId == COMPUTE_VIEW, "Compute pass must use COMPUTE_VIEW");
        AIKO_ASSERT(m_computePassActive, "Compute dispatch requires an active compute pass");
        AIKO_ASSERT(pass.shader != nullptr, "Compute pass has no shader");
        AIKO_ASSERT(pass.shader->isValid(), "Invalid compute shader");
        AIKO_ASSERT(pass.buffers.empty() == false || pass.images.empty() == false, "Vulkan compute requires at least one resource");
        AIKO_ASSERT(pass.buffers.size() <= abi::MaxComputeBufferBindings, "Too many Vulkan compute buffer bindings");

        auto* shaderImpl = static_cast<VulkanComputeShaderImpl*>(pass.shader->getImpl());
        AIKO_ASSERT(shaderImpl != nullptr, "Invalid Vulkan compute shader implementation");

        const VkPipeline pipeline = getOrCreateComputePipeline(*shaderImpl);
        const VkDescriptorSet descriptorSet = allocateComputeDescriptorSet();

        const bool useDedicatedCompute = m_context.hasDedicatedComputeQueue();

        VkCommandBuffer commandBuffer = useDedicatedCompute ? m_context.computeCommandBuffer() : m_context.activeCommandBuffer();
        AIKO_ASSERT(commandBuffer != VK_NULL_HANDLE, "Compute dispatch requires a valid command buffer");

        transitionComputeBuffers(commandBuffer, pass.buffers, useDedicatedCompute);
        transitionComputeImages(commandBuffer, pass.images, useDedicatedCompute);

        VulkanComputeBufferImpl* indirectBufferImpl = nullptr;

        if (pass.dispatch.indirectBuffer != nullptr)
        {

            for (const ComputeBufferBinding& binding : pass.buffers)
            {
                AIKO_ASSERT(binding.buffer != pass.dispatch.indirectBuffer, "Compute indirect dispatch buffer cannot also be a storage binding in the same dispatch");
            }

            AIKO_ASSERT(pass.dispatch.indirectBuffer->isValid(), "Compute indirect dispatch buffer is invalid");

            indirectBufferImpl = static_cast<VulkanComputeBufferImpl*>(pass.dispatch.indirectBuffer->getImpl());

            AIKO_ASSERT(indirectBufferImpl != nullptr, "Compute indirect dispatch buffer has no Vulkan implementation");
            AIKO_ASSERT(indirectBufferImpl->isValid(), "Invalid Vulkan compute indirect dispatch buffer");
            AIKO_ASSERT(hasFlag(indirectBufferImpl->usage(), ComputeBufferUsage::Indirect), "Compute indirect dispatch buffer requires Indirect usage");
            AIKO_ASSERT(indirectBufferImpl->format() == ComputeBufferFormat::Uint32, "Compute indirect dispatch buffer currently requires Uint32 format");
            AIKO_ASSERT(pass.dispatch.indirectOffset % 4 == 0, "Compute indirect dispatch offset must be 4-byte aligned");

            const VkDeviceSize requiredSize = static_cast<VkDeviceSize>(pass.dispatch.indirectOffset) + sizeof(VkDispatchIndirectCommand);
            AIKO_ASSERT(requiredSize <= indirectBufferImpl->size(), "Compute indirect dispatch command exceeds buffer size");

            const uint32_t indirectQueueFamily = useDedicatedCompute ? m_context.computeQueueFamily() : m_context.graphicsQueueFamily();

            const VulkanBufferState destination =
            {
                .stage = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
                .access = VK_ACCESS_INDIRECT_COMMAND_READ_BIT,
                .queueFamily = indirectQueueFamily,
            };

            if (useDedicatedCompute)
            {
                const VulkanBufferState source = indirectBufferImpl->state();
                if (indirectBufferImpl->hasPendingUploads())
                {
                    if (source.queueFamily == m_context.graphicsQueueFamily())
                    {
                        VkCommandBuffer preCompute = m_context.preComputeCommandBuffer();
                        flushComputeBufferUploads(preCompute, *indirectBufferImpl, m_context.graphicsQueueFamily());
                    }
                    else
                    {
                        AIKO_ASSERT(source.queueFamily == VK_QUEUE_FAMILY_IGNORED || source.queueFamily == m_context.computeQueueFamily(), "Compute indirect upload has unexpected queue ownership");
                        flushComputeBufferUploads(commandBuffer, *indirectBufferImpl, m_context.computeQueueFamily());
                    }
                }

                const VulkanBufferState current = indirectBufferImpl->state();

                if (current.queueFamily == VK_QUEUE_FAMILY_IGNORED)
                {
                    indirectBufferImpl->setState(destination);
                }
                else if (current.queueFamily == m_context.computeQueueFamily())
                {
                    transitionBuffer(commandBuffer, *indirectBufferImpl, destination);
                }
                else
                {
                    AIKO_ASSERT(current.queueFamily == m_context.graphicsQueueFamily(), "Compute indirect buffer has unexpected queue ownership");
                    VkCommandBuffer preCompute = m_context.preComputeCommandBuffer();

                    releaseBufferOwnership(preCompute, *indirectBufferImpl, destination);
                    acquireBufferOwnership(commandBuffer, *indirectBufferImpl, current, destination);
                }
            }
            else
            {
                flushComputeBufferUploads(commandBuffer, *indirectBufferImpl, m_context.graphicsQueueFamily());
                transitionBuffer(commandBuffer, *indirectBufferImpl, destination);
            }
        }

        updateComputeDescriptors(descriptorSet, pass.buffers, pass.images);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

        if (pass.pushConstants.empty() == false)
        {
            AIKO_ASSERT(pass.pushConstants.size() <= MaxComputePushConstantBytes, "Compute push constants exceed engine limit");
            AIKO_ASSERT(pass.pushConstants.size() % 4 == 0, "Compute push constant size must be a multiple of 4 bytes");
            vkCmdPushConstants(commandBuffer, m_computePipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, static_cast<uint32_t>( pass.pushConstants.size()), pass.pushConstants.data());
        }

        if (indirectBufferImpl != nullptr)
        {
            vkCmdDispatchIndirect(commandBuffer, indirectBufferImpl->buffer(), pass.dispatch.indirectOffset);
        }
        else
        {
            vkCmdDispatch(commandBuffer, pass.dispatch.groupsX, pass.dispatch.groupsY, pass.dispatch.groupsZ);
        }

    }

    void VulkanRenderDevice::requestReadback(const ComputeReadbackRequest& request)
    {

        AIKO_ASSERT(request.buffer != nullptr, "Compute readback buffer is null");
        AIKO_ASSERT(request.buffer->isValid(), "Compute readback buffer is invalid");
        AIKO_ASSERT(request.byteSize > 0, "Compute readback size must be greater than zero");

        auto* bufferImpl = static_cast<VulkanComputeBufferImpl*>(request.buffer->getImpl());

        AIKO_ASSERT(bufferImpl != nullptr, "Invalid Vulkan compute buffer implementation");
        AIKO_ASSERT(request.byteSize <= bufferImpl->size(), "Compute readback exceeds source buffer size");

        m_readbackRequests.push_back(
        {
            .id = request.id,
            .buffer = request.buffer,
            .byteSize = request.byteSize,
        });
    }

    bool VulkanRenderDevice::pollReadback(ComputeReadbackResult& result)
    {
        if (m_completedReadbacks.empty())
        {
            return false;
        }

        CompletedReadback completed = std::move(m_completedReadbacks.front());

        m_completedReadbacks.pop_front();

        result.id = completed.id;
        result.ready = true;
        result.data = std::move(completed.data);

        return true;
    }

    void VulkanRenderDevice::drawMeshInstancedGpu(ViewId viewId, const GpuInstanceDrawDesc& desc)
    {
        if (viewId != SCENE_VIEW || m_renderPassActive == false)
        {
            return;
        }

        AIKO_ASSERT(desc.mesh != nullptr, "GPU instance draw has no mesh");
        AIKO_ASSERT(desc.material != nullptr, "GPU instance draw has no material");
        AIKO_ASSERT(desc.instanceCount > 0, "GPU instance draw has zero instances");
        AIKO_ASSERT(desc.readBuffers.empty() == false, "GPU instance draw has no GPU-read buffers");

        auto* meshImpl = static_cast<VulkanMeshImpl*>(desc.mesh->getImpl());

        AIKO_ASSERT(meshImpl != nullptr, "GPU-instanced mesh has no Vulkan implementation");
        AIKO_ASSERT(meshImpl->isValid(), "Invalid GPU-instanced mesh");

        const VkDescriptorSet gpuReadSet = buildGpuReadDescriptorSet(desc.readBuffers);

        bindMaterial(*desc.material);

        const VkPipeline pipeline = getOrCreateGpuInstancedPipeline(*desc.material, m_activeRenderPass);
        AIKO_ASSERT(pipeline != VK_NULL_HANDLE, "GPU-instanced pipeline is invalid");

        VkCommandBuffer commandBuffer = m_context.activeCommandBuffer();
        AIKO_ASSERT(commandBuffer != VK_NULL_HANDLE, "GPU-instanced draw requires an active command buffer");

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_modelPipelineLayout, abi::GraphicsGpuReadSet, 1, &gpuReadSet, 0, nullptr);

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

        const mat4 world = mat4(1.0f);

        const DrawPushConstants push =
        {
            .u_model = world,
            .u_modelViewProj = m_sceneViewProj * world,
        };

        vkCmdPushConstants(commandBuffer, m_modelPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(push), &push);

        const VkBuffer vertexBuffers[] =
        {
            meshImpl->vertexBuffer(),
        };

        const VkDeviceSize offsets[] =
        {
            0,
        };

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, meshImpl->indexBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(commandBuffer, meshImpl->indexCount(), desc.instanceCount, 0, 0, 0);

    }

    void VulkanRenderDevice::drawBillboards(ViewId viewId, const GpuBillboardDrawDesc& desc)
    {

        AIKO_ASSERT(desc.material != nullptr, "GPU billboard draw has no material");
        AIKO_ASSERT(desc.positionBuffer != nullptr, "GPU billboard draw has no position buffer");
        AIKO_ASSERT(desc.positionBuffer->isValid(), "GPU billboard position buffer is invalid");
        AIKO_ASSERT(desc.instanceCount > 0, "GPU billboard draw has zero instances");

        static const TransientGeometry billboardGeometry =
        {
            .topology = TransientTopology::Triangles,
            .vertices =
            {
                {
                    .position = vec3(-0.5f, -0.5f, 0.0f),
                    .uv = vec2(0.0f, 1.0f),
                    .normal = vec3(0.0f, 0.0f, 1.0f),
                    .color = WHITE,
                },
                {
                    .position = vec3(0.5f, -0.5f, 0.0f),
                    .uv = vec2(1.0f, 1.0f),
                    .normal = vec3(0.0f, 0.0f, 1.0f),
                    .color = WHITE,
                },
                {
                    .position = vec3(0.5f, 0.5f, 0.0f),
                    .uv = vec2(1.0f, 0.0f),
                    .normal = vec3(0.0f, 0.0f, 1.0f),
                    .color = WHITE,
                },
                {
                    .position = vec3(-0.5f, 0.5f, 0.0f),
                    .uv = vec2(0.0f, 0.0f),
                    .normal = vec3(0.0f, 0.0f, 1.0f),
                    .color = WHITE,
                },
            },
            .indices =
            {
                0, 1, 2,
                0, 2, 3,
            },
        };

        Mesh& mesh = resolveTransientMesh(billboardGeometry);

        const GpuInstanceDrawDesc draw =
        {
            .mesh = &mesh,
            .material = desc.material,
            .readBuffers =
            {
                {
                    .slot = 7,
                    .buffer = desc.positionBuffer,
                }
            },
            .instanceCount = desc.instanceCount,
        };

        drawMeshInstancedGpu(viewId, draw);
    }

    void VulkanRenderDevice::drawVerticesGpu(ViewId viewId, const GpuVertexDrawDesc& desc)
    {
        if (viewId != SCENE_VIEW || m_renderPassActive == false)
        {
            return;
        }

        AIKO_ASSERT(desc.material != nullptr, "GPU vertex draw has no material");
        AIKO_ASSERT(desc.vertexBuffer != nullptr, "GPU vertex draw has no vertex buffer");
        AIKO_ASSERT(desc.vertexBuffer->isValid(), "GPU vertex buffer is invalid");
        AIKO_ASSERT(desc.vertexCount > 0, "GPU vertex draw has zero vertices");

        auto* bufferImpl = static_cast<VulkanComputeBufferImpl*>(desc.vertexBuffer->getImpl());
        AIKO_ASSERT(bufferImpl != nullptr, "GPU vertex buffer has no Vulkan implementation");
        AIKO_ASSERT(bufferImpl->isValid(), "Invalid Vulkan GPU vertex buffer");
        AIKO_ASSERT(hasFlag(bufferImpl->usage(), ComputeBufferUsage::Vertex), "GPU vertex buffer requires Vertex usage");
        AIKO_ASSERT(bufferImpl->format() == ComputeBufferFormat::Vec4f, "GPU vertex draw currently requires Vec4f format");
        AIKO_ASSERT(desc.vertexCount <= bufferImpl->count(), "GPU vertex draw exceeds compute buffer element count");

        VulkanComputeBufferImpl* indexBufferImpl = nullptr;

        if (desc.indexBuffer != nullptr)
        {
            AIKO_ASSERT(desc.indexBuffer->isValid(), "GPU index buffer is invalid");
            AIKO_ASSERT(desc.indexCount > 0, "GPU indexed draw has zero indices");

            indexBufferImpl = static_cast<VulkanComputeBufferImpl*>(desc.indexBuffer->getImpl());

            AIKO_ASSERT(indexBufferImpl != nullptr, "GPU index buffer has no Vulkan implementation");
            AIKO_ASSERT(indexBufferImpl->isValid(), "Invalid Vulkan GPU index buffer");
            AIKO_ASSERT(hasFlag(indexBufferImpl->usage(), ComputeBufferUsage::Index), "GPU index buffer requires Index usage");
            AIKO_ASSERT(indexBufferImpl->format() == ComputeBufferFormat::Uint32, "GPU index buffer currently requires Uint32 format");
            AIKO_ASSERT(desc.indexCount <= indexBufferImpl->count(), "GPU indexed draw exceeds compute index buffer element count");
        }

        VulkanComputeBufferImpl* indirectBufferImpl = nullptr;

        if (desc.indirectBuffer != nullptr)
        {
            AIKO_ASSERT(desc.indirectBuffer->isValid(), "GPU indirect buffer is invalid");

            indirectBufferImpl = static_cast<VulkanComputeBufferImpl*>(desc.indirectBuffer->getImpl());
            AIKO_ASSERT(indirectBufferImpl != nullptr, "GPU indirect buffer has no Vulkan implementation");
            AIKO_ASSERT(indirectBufferImpl->isValid(), "Invalid Vulkan GPU indirect buffer");
            AIKO_ASSERT(hasFlag(indirectBufferImpl->usage(), ComputeBufferUsage::Indirect), "GPU indirect buffer requires Indirect usage");
            AIKO_ASSERT(indirectBufferImpl->format() == ComputeBufferFormat::Uint32, "GPU indirect buffer currently requires Uint32 format");

            const uint32_t requiredWords = indexBufferImpl != nullptr ? 5u : 4u;
            AIKO_ASSERT(indirectBufferImpl->count() >= requiredWords, "GPU indirect buffer is too small for draw command");

        }

        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

        switch (desc.topology)
        {
            case TransientTopology::Points:
                topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                break;

            case TransientTopology::Lines:
                topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                break;

            case TransientTopology::Triangles:
                topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                break;
        }

        bindMaterial(*desc.material);

        const VkPipeline pipeline = getOrCreateGpuVertexPipeline(*desc.material, m_activeRenderPass, topology);
        AIKO_ASSERT(pipeline != VK_NULL_HANDLE, "GPU vertex pipeline is invalid");

        VkCommandBuffer commandBuffer = m_context.activeCommandBuffer();
        AIKO_ASSERT(commandBuffer != VK_NULL_HANDLE, "GPU vertex draw requires an active command buffer");

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

        vkCmdSetViewport(commandBuffer,0,1,&viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        struct DrawPushConstants
        {
            mat4 u_model;
            mat4 u_modelViewProj;
        };

        const mat4 world = mat4(1.0f);

        const DrawPushConstants push =
        {
            .u_model = world,
            .u_modelViewProj = m_sceneViewProj * world,
        };

        vkCmdPushConstants(commandBuffer, m_modelPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(push), &push);

        const VkBuffer vertexBuffer = bufferImpl->buffer();
        const VkDeviceSize offset = 0;

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);

        if (indexBufferImpl != nullptr)
        {
            vkCmdBindIndexBuffer(commandBuffer, indexBufferImpl->buffer(), 0, VK_INDEX_TYPE_UINT32);

            if (indirectBufferImpl != nullptr)
            {
                vkCmdDrawIndexedIndirect(commandBuffer, indirectBufferImpl->buffer(), 0, 1, sizeof(VkDrawIndexedIndirectCommand));
            }
            else
            {
                vkCmdDrawIndexed(commandBuffer, desc.indexCount, 1, 0, 0, 0);
            }
        }
        else
        {
            if (indirectBufferImpl != nullptr)
            {
                vkCmdDrawIndirect(commandBuffer, indirectBufferImpl->buffer(), 0, 1, sizeof(VkDrawIndirectCommand));
            }
            else
            {
                vkCmdDraw(commandBuffer, desc.vertexCount, 1, 0, 0);
            }
        }

    }

    void VulkanRenderDevice::prepareVertexBuffer(const ComputeBuffer& buffer)
    {
        auto* impl = static_cast<VulkanComputeBufferImpl*>(buffer.getImpl());
        AIKO_ASSERT(impl != nullptr, "Invalid Vulkan compute buffer implementation");
        AIKO_ASSERT(impl->isValid(), "Invalid Vulkan vertex compute buffer");
        AIKO_ASSERT(hasFlag(impl->usage(), ComputeBufferUsage::Vertex), "Compute buffer requires Vertex usage");

        VkCommandBuffer commandBuffer = m_context.activeCommandBuffer();
        AIKO_ASSERT(commandBuffer != VK_NULL_HANDLE, "Vertex buffer preparation requires an active frame command buffer");
        AIKO_ASSERT(m_renderPassActive == false, "Vertex buffer preparation must happen outside a render pass");

        const VulkanBufferState destination =
        {
            .stage = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
            .access = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
            .queueFamily = m_context.graphicsQueueFamily(),
        };

        prepareBufferForGraphics( *impl, destination);

    }

    void VulkanRenderDevice::prepareIndexBuffer(const ComputeBuffer& buffer)
    {

        auto* impl = static_cast<VulkanComputeBufferImpl*>(buffer.getImpl());
        AIKO_ASSERT(impl != nullptr, "Invalid Vulkan compute buffer implementation");
        AIKO_ASSERT(impl->isValid(), "Invalid Vulkan index compute buffer");
        AIKO_ASSERT(hasFlag(impl->usage(), ComputeBufferUsage::Index), "Compute buffer requires Index usage");
        AIKO_ASSERT(impl->format() == ComputeBufferFormat::Uint32, "Vulkan GPU index buffer currently requires Uint32 format");

        VkCommandBuffer commandBuffer = m_context.activeCommandBuffer();

        AIKO_ASSERT( commandBuffer != VK_NULL_HANDLE, "Index buffer preparation requires an active frame command buffer");
        AIKO_ASSERT(m_renderPassActive == false, "Index buffer preparation must happen outside a render pass");

        const VulkanBufferState destination =
        {
            .stage = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
            .access = VK_ACCESS_INDEX_READ_BIT,
            .queueFamily = m_context.graphicsQueueFamily(),
        };

        prepareBufferForGraphics(*impl, destination);

    }

    void VulkanRenderDevice::prepareIndirectBuffer(const ComputeBuffer& buffer)
    {
        auto* impl = static_cast<VulkanComputeBufferImpl*>(buffer.getImpl());
        AIKO_ASSERT(impl != nullptr, "Invalid Vulkan compute buffer implementation");
        AIKO_ASSERT(impl->isValid(), "Invalid Vulkan indirect compute buffer");
        AIKO_ASSERT(hasFlag(impl->usage(), ComputeBufferUsage::Indirect), "Compute buffer requires Indirect usage");
        AIKO_ASSERT(impl->format() == ComputeBufferFormat::Uint32, "Vulkan indirect buffer currently requires Uint32 format");

        VkCommandBuffer commandBuffer = m_context.activeCommandBuffer();
        AIKO_ASSERT(commandBuffer != VK_NULL_HANDLE, "Indirect buffer preparation requires an active frame command buffer");
        AIKO_ASSERT(m_renderPassActive == false, "Indirect buffer preparation must happen outside a render pass");

        const VulkanBufferState destination =
        {
            .stage = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
            .access = VK_ACCESS_INDIRECT_COMMAND_READ_BIT,
            .queueFamily = m_context.graphicsQueueFamily(),
        };

        prepareBufferForGraphics(*impl, destination);

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

    void VulkanRenderDevice::prepareTextureForSampling(const Texture& texture)
    {
        AIKO_ASSERT(m_frameActive, "Texture preparation requires an active frame");
        AIKO_ASSERT(m_renderPassActive == false, "Texture preparation must happen outside a render pass");
        AIKO_ASSERT(texture.isValid(), "Cannot prepare invalid texture");

        auto* textureImpl = static_cast<VulkanTextureImpl*>(texture.getImpl());

        AIKO_ASSERT(textureImpl != nullptr, "Invalid Vulkan texture implementation");

        const VulkanImageState sampledState =
        {
            .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .access = VK_ACCESS_SHADER_READ_BIT,
            .queueFamily = m_context.graphicsQueueFamily(),
        };

        const VulkanImageState source = textureImpl->state();

        VkCommandBuffer graphicsCommandBuffer = m_context.activeCommandBuffer();
        AIKO_ASSERT(graphicsCommandBuffer != VK_NULL_HANDLE, "Texture sampling preparation requires an active graphics command buffer");

        if (source.queueFamily == VK_QUEUE_FAMILY_IGNORED)
        {
            transitionTexture(graphicsCommandBuffer, *textureImpl, sampledState);
            return;
        }

        if (source.queueFamily == m_context.graphicsQueueFamily())
        {
            transitionTexture(graphicsCommandBuffer, *textureImpl, sampledState);
            return;
        }

        AIKO_ASSERT(m_context.hasDedicatedComputeQueue(), "Texture is compute-owned without a dedicated compute queue");
        AIKO_ASSERT(source.queueFamily == m_context.computeQueueFamily(), "Sampled texture has unexpected queue ownership");

        VkCommandBuffer computeCommandBuffer = m_context.computeCommandBuffer();
        AIKO_ASSERT(computeCommandBuffer != VK_NULL_HANDLE, "Texture sampling preparation requires a compute command buffer");

        releaseTextureOwnership(computeCommandBuffer, *textureImpl, sampledState);
        acquireTextureOwnership(graphicsCommandBuffer, *textureImpl, source, sampledState);
    }

    void VulkanRenderDevice::prepareMaterial(const Material& material)
    {
        const Texture* texture = resolveMaterialTexture(material);
        AIKO_ASSERT(texture != nullptr, "Failed to resolve material texture");
        prepareTextureForSampling(*texture);
    }

    const Texture* VulkanRenderDevice::resolveMaterialTexture(const Material& material)
    {
        if (material.m_runtimeDiffuseTexture != nullptr && material.m_runtimeDiffuseTexture->isValid())
        {
            return material.m_runtimeDiffuseTexture;
        }

        if (material.m_diffuseTextureId != InvalidAssetId)
        {
            Texture& texture = getResources()->getTexture(material.m_diffuseTextureId);
            if (texture.isValid())
            {
                return &texture;
            }
        }

        return &m_whiteTexture;
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

        const std::array<uint32_t, 2> frameQueueFamilies =
        {
            m_context.graphicsQueueFamily(),
            m_context.computeQueueFamily(),
        };

        for (size_t i = 0; i < FramesInFlight; ++i)
        {

            if (m_context.hasDedicatedComputeQueue())
            {
                m_context.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_frameUniformBuffers[i], m_frameUniformMemories[i], VK_SHARING_MODE_CONCURRENT, frameQueueFamilies.data(), static_cast<uint32_t>(frameQueueFamilies.size()));
            }
            else
            {
                m_context.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_frameUniformBuffers[i], m_frameUniformMemories[i]);
            }

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
                .dstBinding = abi::GraphicsFrameBinding,
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

    void VulkanRenderDevice::createModelInstancedPipeline(VkRenderPass renderPass)
    {
        AIKO_ASSERT(renderPass != VK_NULL_HANDLE, "Model instanced render pass is invalid");
        AIKO_ASSERT(m_modelPipelineLayout != VK_NULL_HANDLE, "Model pipeline layout is invalid");

        VulkanShaderImpl shader;
        shader.load("model_instanced.vs", "model_instanced.fs");

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

        const VkVertexInputBindingDescription meshBinding =
            VulkanVertex::bindingDescription();

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

        std::array<VkVertexInputAttributeDescription, 8> attributes{};

        for (size_t i = 0; i < meshAttributes.size(); ++i)
        {
            attributes[i] = meshAttributes[i];
        }

        attributes[4] =
        {
            .location = 4,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(VulkanInstanceData, position),
        };

        attributes[5] =
        {
            .location = 5,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(VulkanInstanceData, rotation),
        };

        attributes[6] =
        {
            .location = 6,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(VulkanInstanceData, scale),
        };

        attributes[7] =
        {
            .location = 7,
            .binding = 1,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(VulkanInstanceData, color),
        };

        const VkPipelineVertexInputStateCreateInfo vertexInputInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size()),
            .pVertexBindingDescriptions = bindings.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size()),
            .pVertexAttributeDescriptions = attributes.data(),
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

        const VkResult result = vkCreateGraphicsPipelines(
            m_context.device(),
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            nullptr,
            &m_modelInstancedPipeline
        );

        shader.unload();

        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create model instanced graphics pipeline");
    }

    void VulkanRenderDevice::createModelPipelineLayout()
    {
        const VkDescriptorSetLayoutBinding frameBinding =
        {
            .binding = abi::GraphicsFrameBinding,
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
                .binding = abi::MaterialUboBinding,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            },
            VkDescriptorSetLayoutBinding
            {
                .binding = abi::MaterialTextureBinding,
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

        std::array<VkDescriptorSetLayout, 3> setLayouts{};
        setLayouts[abi::GraphicsFrameSet] = m_frameDescriptorSetLayout;
        setLayouts[abi::GraphicsMaterialSet] = m_materialDescriptorSetLayout;
        setLayouts[abi::GraphicsGpuReadSet] = m_gpuReadDescriptorSetLayout;

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

        if (m_modelInstancedPipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline(device, m_modelInstancedPipeline, nullptr);
            m_modelInstancedPipeline = VK_NULL_HANDLE;
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

        const Texture* texture = resolveMaterialTexture(material);
        AIKO_ASSERT(texture != nullptr, "Failed to resolve material texture");

        const bool hasRealTexture = texture != &m_whiteTexture;

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
                .dstBinding = abi::MaterialUboBinding,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &bufferInfo,
            },
            VkWriteDescriptorSet
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = binding.descriptorSet,
                .dstBinding = abi::MaterialTextureBinding,
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

        std::array<VkDescriptorSetLayoutBinding, abi::MaxComputeBufferBindings + abi::MaxComputeImageBindings + 1> bindings{};

        for (uint32_t i = 0; i < abi::MaxComputeBufferBindings; ++i)
        {
            bindings[i] =
            {
                .binding = i,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                .pImmutableSamplers = nullptr,
            };
        }

        for (uint32_t i = 0; i < abi::MaxComputeImageBindings; ++i)
        {
            const uint32_t bindingIndex = abi::ComputeImageBindingBase + i;
            bindings[bindingIndex] =
            {
                .binding = bindingIndex,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                .pImmutableSamplers = nullptr,
            };
        }

        bindings[abi::ComputeFrameBinding] =
        {
            .binding = abi::ComputeFrameBinding,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = nullptr,
        };

        const VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data(),
        };

        const VkResult descriptorResult = vkCreateDescriptorSetLayout(m_context.device(), &descriptorLayoutInfo, nullptr, &m_computeDescriptorSetLayout);
        AIKO_ASSERT(descriptorResult == VK_SUCCESS, "Failed to create compute descriptor set layout");

        const VkPushConstantRange pushConstantRange =
        {
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .offset = 0,
            .size = MaxComputePushConstantBytes,
        };

        const VkPipelineLayoutCreateInfo pipelineLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 1,
            .pSetLayouts = &m_computeDescriptorSetLayout,
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &pushConstantRange,
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

    VkPipeline VulkanRenderDevice::getOrCreateComputePipeline(const VulkanComputeShaderImpl& shader)
    {
        const VkShaderModule shaderModule = shader.module();
        AIKO_ASSERT(shaderModule != VK_NULL_HANDLE, "Invalid compute shader module");

        const auto it = m_computePipelines.find(shaderModule);

        if (it != m_computePipelines.end())
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
            .layout = m_computePipelineLayout,
        };

        VkPipeline pipeline = VK_NULL_HANDLE;

        const VkResult result = vkCreateComputePipelines(m_context.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan compute pipeline");

        m_computePipelines.emplace(shaderModule, pipeline);

        return pipeline;

    }

    void VulkanRenderDevice::destroyComputePipelines()
    {
        VkDevice device = m_context.device();
        for (const auto& [shaderModule, pipeline] : m_computePipelines)
        {
            AIKO_UNUSED(shaderModule);
            if (pipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(device, pipeline, nullptr);
            }
        }

        m_computePipelines.clear();
    }

    void VulkanRenderDevice::createComputeDescriptorPools()
    {
        for (size_t frame = 0; frame < FramesInFlight; ++frame)
        {
            const std::array<VkDescriptorPoolSize, 3> poolSizes =
            {
                VkDescriptorPoolSize
                {
                    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    .descriptorCount = abi::MaxComputeBufferBindings * MaxComputeDispatchesPerFrame,
                },
                VkDescriptorPoolSize
                {
                    .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                    .descriptorCount = abi::MaxComputeImageBindings * MaxComputeDispatchesPerFrame,
                },
                VkDescriptorPoolSize
                {
                    .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = MaxComputeDispatchesPerFrame,
                }
            };

            const VkDescriptorPoolCreateInfo poolInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .maxSets = MaxComputeDispatchesPerFrame,
                .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
                .pPoolSizes = poolSizes.data(),
            };

            const VkResult result = vkCreateDescriptorPool(m_context.device(), &poolInfo, nullptr, &m_computeDescriptorPools[frame]);
            AIKO_ASSERT(result == VK_SUCCESS, "Failed to create compute descriptor pool");

        }
    }

    VkDescriptorSet VulkanRenderDevice::allocateComputeDescriptorSet()
    {
        const uint32_t frame = m_context.currentFrameIndex();

        AIKO_ASSERT(frame < FramesInFlight, "Invalid Vulkan frame index");
        AIKO_ASSERT(m_computeDescriptorPools[frame] != VK_NULL_HANDLE, "Compute descriptor pool is invalid");

        const VkDescriptorSetAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_computeDescriptorPools[frame],
            .descriptorSetCount = 1,
            .pSetLayouts = &m_computeDescriptorSetLayout,
        };

        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

        const VkResult result = vkAllocateDescriptorSets(m_context.device(), &allocInfo, &descriptorSet);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to allocate compute descriptor set");

        return descriptorSet;
    }

    void VulkanRenderDevice::destroyComputeDescriptorPools()
    {
        VkDevice device = m_context.device();
        for (VkDescriptorPool& pool : m_computeDescriptorPools)
        {
            if (pool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(device, pool, nullptr);
                pool = VK_NULL_HANDLE;
            }
        }
    }

    void VulkanRenderDevice::updateComputeDescriptors(VkDescriptorSet descriptorSet, const std::vector<ComputeBufferBinding>& bindings, const std::vector<ComputeImageBinding>& images)
    {

        AIKO_ASSERT(bindings.size() <= abi::MaxComputeBufferBindings, "Too many Vulkan compute buffer bindings");

        std::array<VkDescriptorBufferInfo, abi::MaxComputeBufferBindings> bufferInfos{};
        std::array<VkWriteDescriptorSet, abi::MaxComputeBufferBindings> writes{};
        std::array<VkDescriptorImageInfo, abi::MaxComputeImageBindings> imageInfos{};
        std::array<VkWriteDescriptorSet, abi::MaxComputeImageBindings> imageWrites{};
        std::array<bool, abi::MaxComputeBufferBindings> usedBindings{};

        uint32_t writeCount = 0;

        for (const ComputeBufferBinding& binding : bindings)
        {
            AIKO_ASSERT(binding.stage < abi::MaxComputeBufferBindings, "Compute buffer binding exceeds Vulkan binding limit");
            AIKO_ASSERT(usedBindings[binding.stage] == false, "Duplicate Vulkan compute buffer binding");
            AIKO_ASSERT(binding.buffer != nullptr, "Compute buffer binding is null");
            AIKO_ASSERT(binding.buffer->isValid(), "Invalid compute buffer");

            auto* bufferImpl = static_cast<VulkanComputeBufferImpl*>(binding.buffer->getImpl());
            AIKO_ASSERT(bufferImpl != nullptr, "Invalid Vulkan compute buffer implementation");

            AIKO_ASSERT(hasFlag(bufferImpl->usage(),ComputeBufferUsage::Storage), "Compute binding requires storage-buffer usage");

            usedBindings[binding.stage] = true;

            bufferInfos[writeCount] =
            {
                .buffer = bufferImpl->buffer(),
                .offset = 0,
                .range = bufferImpl->size(),
            };

            writes[writeCount] =
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descriptorSet,
                .dstBinding = binding.stage,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pBufferInfo = &bufferInfos[writeCount],
            };

            ++writeCount;
        }

        uint32_t imageWriteCount = 0;

        for (const ComputeImageBinding& binding : images)
        {
            AIKO_ASSERT(binding.stage < abi::MaxComputeImageBindings, "Compute image binding exceeds Vulkan binding limit");
            AIKO_ASSERT(binding.texture != nullptr, "Compute image texture is null");
            AIKO_ASSERT(binding.texture->isValid(), "Invalid compute image texture");

            auto* textureImpl = static_cast<VulkanTextureImpl*>(binding.texture->getImpl());

            AIKO_ASSERT(textureImpl != nullptr, "Invalid Vulkan compute texture");

            imageInfos[imageWriteCount] =
            {
                .sampler = VK_NULL_HANDLE,
                .imageView = textureImpl->imageView(),
                .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
            };

            imageWrites[imageWriteCount] =
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descriptorSet,
                .dstBinding = abi::ComputeImageBindingBase + binding.stage,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .pImageInfo = &imageInfos[imageWriteCount],
            };

            ++imageWriteCount;
        }

        const uint32_t frame = m_context.currentFrameIndex();

        AIKO_ASSERT(frame < FramesInFlight, "Invalid Vulkan frame index");

        const VkDescriptorBufferInfo frameInfo =
        {
            .buffer = m_frameUniformBuffers[frame],
            .offset = 0,
            .range = sizeof(VulkanFrameUbo),
        };

        const VkWriteDescriptorSet frameWrite =
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSet,
            .dstBinding = abi::ComputeFrameBinding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &frameInfo,
        };

        std::vector<VkWriteDescriptorSet> allWrites;
        allWrites.reserve(writeCount + imageWriteCount + 1);

        for (uint32_t i = 0; i < writeCount; ++i)
        {
            allWrites.push_back(writes[i]);
        }

        for (uint32_t i = 0; i < imageWriteCount; ++i)
        {
            allWrites.push_back(imageWrites[i]);
        }

        allWrites.push_back(frameWrite);

        vkUpdateDescriptorSets(m_context.device(), static_cast<uint32_t>(allWrites.size()), allWrites.data(), 0, nullptr);

    }

    void VulkanRenderDevice::transitionComputeImages(VkCommandBuffer commandBuffer, const vector<ComputeImageBinding>& bindings, bool useDedicatedCompute)
    {
        for (const ComputeImageBinding& binding : bindings)
        {
            AIKO_ASSERT(binding.texture != nullptr, "Compute image texture is null");

            auto* textureImpl = static_cast<VulkanTextureImpl*>(binding.texture->getImpl());
            AIKO_ASSERT(textureImpl != nullptr, "Invalid Vulkan compute texture");

            const uint32_t queueFamily = useDedicatedCompute ? m_context.computeQueueFamily() : m_context.graphicsQueueFamily();

            const VulkanImageState destination = computeImageState(binding.access, queueFamily);

            if (useDedicatedCompute == false)
            {
                transitionTexture(commandBuffer, *textureImpl, destination);
                continue;
            }

            const VulkanImageState source = textureImpl->state();

            if (source.queueFamily == VK_QUEUE_FAMILY_IGNORED)
            {

                const VulkanImageState initialState =
                {
                    .layout = source.layout,
                    .stage = source.stage,
                    .access = source.access,
                    .queueFamily = m_context.computeQueueFamily(),
                };

                textureImpl->setState(initialState);

                transitionTexture(commandBuffer, *textureImpl, destination);

                continue;
            }

            if (source.queueFamily == m_context.computeQueueFamily())
            {
                transitionTexture(commandBuffer, *textureImpl, destination);

                continue;
            }

            AIKO_ASSERT(source.queueFamily == m_context.graphicsQueueFamily(), "Compute image has unexpected queue ownership");

            VkCommandBuffer preCompute = m_context.preComputeCommandBuffer();

            releaseTextureOwnership(preCompute, *textureImpl, destination);
            acquireTextureOwnership(commandBuffer, *textureImpl, source, destination);
        }
    }

    void VulkanRenderDevice::transitionTexture(VkCommandBuffer commandBuffer, VulkanTextureImpl& texture, const VulkanImageState& destination)
    {
        const VulkanImageState source = texture.state();

        const bool sameLayout = source.layout == destination.layout;

        const bool noWriteHazard = (source.access & (VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) == 0;

        const bool destinationOnlyReads = (destination.access & (VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) == 0;

        const bool sameQueueFamily =
            source.queueFamily == destination.queueFamily;

        if (sameLayout && noWriteHazard && destinationOnlyReads && sameQueueFamily)
        {
            texture.setState(
            {
                .layout = destination.layout,
                .stage = source.stage | destination.stage,
                .access = source.access | destination.access,
                .queueFamily = destination.queueFamily,
            });
            return;
        }

        const VkImageMemoryBarrier barrier =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,

            .srcAccessMask = source.access,
            .dstAccessMask = destination.access,

            .oldLayout = source.layout,
            .newLayout = destination.layout,

            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

            .image = texture.image(),

            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = VK_REMAINING_MIP_LEVELS,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        vkCmdPipelineBarrier( commandBuffer, source.stage, destination.stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        texture.setState(destination);
    }

    void VulkanRenderDevice::transitionBuffer(VkCommandBuffer commandBuffer, VulkanComputeBufferImpl& buffer, const VulkanBufferState& destination)
    {
        const VulkanBufferState source = buffer.state();

        constexpr VkAccessFlags writeAccess = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_HOST_WRITE_BIT;

        const bool sourceWrites = (source.access & writeAccess) != 0;

        const bool destinationWrites = (destination.access & writeAccess) != 0;

        if (sourceWrites == false && destinationWrites == false && source.queueFamily == destination.queueFamily)
        {
            buffer.setState(
            {
                .stage = source.stage | destination.stage,
                .access = source.access | destination.access,
                .queueFamily = destination.queueFamily,
            });

            return;
        }

        const VkBufferMemoryBarrier barrier =
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .srcAccessMask = source.access,
            .dstAccessMask = destination.access,

            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

            .buffer = buffer.buffer(),
            .offset = 0,
            .size = buffer.size(),
        };

        vkCmdPipelineBarrier(commandBuffer, source.stage, destination.stage, 0, 0, nullptr, 1, &barrier, 0, nullptr);

        buffer.setState(destination);
    }

    void VulkanRenderDevice::releaseBufferOwnership(VkCommandBuffer commandBuffer, VulkanComputeBufferImpl& buffer, const VulkanBufferState& destination)
    {
        const VulkanBufferState source = buffer.state();

        AIKO_ASSERT(source.queueFamily != VK_QUEUE_FAMILY_IGNORED, "Buffer ownership release requires a source queue family");
        AIKO_ASSERT(destination.queueFamily != VK_QUEUE_FAMILY_IGNORED, "Buffer ownership release requires a destination queue family");
        AIKO_ASSERT(source.queueFamily != destination.queueFamily, "Buffer ownership release requires different queue families");

        const VkBufferMemoryBarrier barrier =
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .srcAccessMask = source.access,
            .dstAccessMask = 0,
            .srcQueueFamilyIndex = source.queueFamily,
            .dstQueueFamilyIndex = destination.queueFamily,
            .buffer = buffer.buffer(),
            .offset = 0,
            .size = buffer.size(),
        };

        vkCmdPipelineBarrier(commandBuffer, source.stage, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 1, &barrier, 0, nullptr);
    }

    void VulkanRenderDevice::acquireBufferOwnership(VkCommandBuffer commandBuffer, VulkanComputeBufferImpl& buffer, const VulkanBufferState& source, const VulkanBufferState& destination)
    {
        AIKO_ASSERT(source.queueFamily != VK_QUEUE_FAMILY_IGNORED, "Buffer ownership acquire requires a source queue family");
        AIKO_ASSERT(destination.queueFamily != VK_QUEUE_FAMILY_IGNORED, "Buffer ownership acquire requires a destination queue family");
        AIKO_ASSERT(source.queueFamily != destination.queueFamily, "Buffer ownership acquire requires different queue families");

        const VkBufferMemoryBarrier barrier =
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,

            .srcAccessMask = 0,
            .dstAccessMask = destination.access,

            .srcQueueFamilyIndex = source.queueFamily,
            .dstQueueFamilyIndex = destination.queueFamily,

            .buffer = buffer.buffer(),
            .offset = 0,
            .size = buffer.size(),
        };

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, destination.stage, 0, 0, nullptr, 1, &barrier, 0, nullptr);

        buffer.setState(destination);
    }

    void VulkanRenderDevice::releaseTextureOwnership(VkCommandBuffer commandBuffer, VulkanTextureImpl& texture, const VulkanImageState& destination)
    {
        const VulkanImageState source = texture.state();

        AIKO_ASSERT(source.queueFamily != VK_QUEUE_FAMILY_IGNORED, "Image ownership release requires a source queue family");
        AIKO_ASSERT(destination.queueFamily != VK_QUEUE_FAMILY_IGNORED, "Image ownership release requires a destination queue family");
        AIKO_ASSERT(source.queueFamily != destination.queueFamily, "Image ownership release requires different queue families");

        const VkImageMemoryBarrier barrier =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,

            .srcAccessMask = source.access,
            .dstAccessMask = 0,

            .oldLayout = source.layout,
            .newLayout = destination.layout,

            .srcQueueFamilyIndex = source.queueFamily,
            .dstQueueFamilyIndex = destination.queueFamily,

            .image = texture.image(),

            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = VK_REMAINING_MIP_LEVELS,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        vkCmdPipelineBarrier(commandBuffer, source.stage, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    void VulkanRenderDevice::acquireTextureOwnership(VkCommandBuffer commandBuffer, VulkanTextureImpl& texture, const VulkanImageState& source, const VulkanImageState& destination)
    {
        AIKO_ASSERT(source.queueFamily != VK_QUEUE_FAMILY_IGNORED, "Image ownership acquire requires a source queue family");
        AIKO_ASSERT(destination.queueFamily != VK_QUEUE_FAMILY_IGNORED, "Image ownership acquire requires a destination queue family");
        AIKO_ASSERT(source.queueFamily != destination.queueFamily, "Image ownership acquire requires different queue families");

        const VkImageMemoryBarrier barrier =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,

            .srcAccessMask = 0,
            .dstAccessMask = destination.access,

            .oldLayout = source.layout,
            .newLayout = destination.layout,

            .srcQueueFamilyIndex = source.queueFamily,
            .dstQueueFamilyIndex = destination.queueFamily,

            .image = texture.image(),

            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = VK_REMAINING_MIP_LEVELS,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, destination.stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        texture.setState(destination);
    }

    void VulkanRenderDevice::transitionComputeBuffers(VkCommandBuffer commandBuffer, const vector<ComputeBufferBinding>& bindings, bool useDedicatedCompute)
    {
        for (const ComputeBufferBinding& binding : bindings)
        {
            AIKO_ASSERT(binding.buffer != nullptr, "Compute buffer binding is null");
            AIKO_ASSERT(binding.buffer->isValid(), "Invalid compute buffer");

            auto* bufferImpl = static_cast<VulkanComputeBufferImpl*>(binding.buffer->getImpl());

            AIKO_ASSERT(bufferImpl != nullptr, "Invalid Vulkan compute buffer implementation");

            const uint32_t queueFamily = useDedicatedCompute ? m_context.computeQueueFamily() : m_context.graphicsQueueFamily();

            const VulkanBufferState destination = computeBufferState(binding.access, queueFamily);

            if (useDedicatedCompute)
            {
                const VulkanBufferState source = bufferImpl->state();

                if (bufferImpl->hasPendingUploads())
                {
                    if (source.queueFamily == m_context.graphicsQueueFamily())
                    {
                        VkCommandBuffer preCompute = m_context.preComputeCommandBuffer();
                        flushComputeBufferUploads(preCompute, *bufferImpl, m_context.graphicsQueueFamily());
                    }
                    else
                    {
                        AIKO_ASSERT(source.queueFamily == VK_QUEUE_FAMILY_IGNORED || source.queueFamily == m_context.computeQueueFamily(), "Compute upload buffer has unexpected queue ownership");
                        flushComputeBufferUploads(commandBuffer, *bufferImpl, m_context.computeQueueFamily());
                    }
                }

                const VulkanBufferState current = bufferImpl->state();

                if (current.queueFamily == VK_QUEUE_FAMILY_IGNORED)
                {
                    bufferImpl->setState(destination);
                    continue;
                }

                if (current.queueFamily == m_context.computeQueueFamily())
                {
                    transitionBuffer(commandBuffer, *bufferImpl, destination);
                    continue;
                }

                AIKO_ASSERT(current.queueFamily == m_context.graphicsQueueFamily(), "Compute buffer has unexpected queue ownership");

                VkCommandBuffer preCompute = m_context.preComputeCommandBuffer();

                releaseBufferOwnership(preCompute, *bufferImpl, destination);
                acquireBufferOwnership(commandBuffer, *bufferImpl, current, destination);

            }
            else
            {
                flushComputeBufferUploads(commandBuffer, *bufferImpl, m_context.graphicsQueueFamily());
                transitionBuffer(commandBuffer, *bufferImpl, destination);
            }
        }
    }

    VulkanBufferState VulkanRenderDevice::computeBufferState(ComputeAccess access, uint32_t queueFamily) const
    {
        VkAccessFlags accessMask = 0;

        switch (access)
        {
            case ComputeAccess::Read:
                accessMask = VK_ACCESS_SHADER_READ_BIT;
                break;

            case ComputeAccess::Write:
                accessMask = VK_ACCESS_SHADER_WRITE_BIT;
                break;

            case ComputeAccess::ReadWrite:
                accessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
                break;
        }

        return
        {
            .stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            .access = accessMask,
            .queueFamily = queueFamily,
        };
    }

    VulkanImageState VulkanRenderDevice::computeImageState(ComputeAccess access, uint32_t queueFamily) const
    {
        VkAccessFlags accessMask = 0;

        switch (access)
        {
            case ComputeAccess::Read:
                accessMask = VK_ACCESS_SHADER_READ_BIT;
                break;

            case ComputeAccess::Write:
                accessMask = VK_ACCESS_SHADER_WRITE_BIT;
                break;

            case ComputeAccess::ReadWrite:
                accessMask =
                    VK_ACCESS_SHADER_READ_BIT |
                    VK_ACCESS_SHADER_WRITE_BIT;
                break;
        }

        return
        {
            .layout = VK_IMAGE_LAYOUT_GENERAL,
            .stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            .access = accessMask,
            .queueFamily = queueFamily,
        };
    }

    void VulkanRenderDevice::flushComputeBufferUploads(VkCommandBuffer commandBuffer, VulkanComputeBufferImpl& buffer, uint32_t queueFamily)
    {

        if (buffer.hasPendingUploads() == false)
        {
            return;
        }

        AIKO_ASSERT(hasFlag(buffer.usage(), ComputeBufferUsage::TransferDst), "Compute buffer upload requires transfer-destination usage");
        AIKO_ASSERT(queueFamily == m_context.graphicsQueueFamily() || queueFamily == m_context.computeQueueFamily(), "Compute buffer upload uses unexpected queue family");

        const VulkanBufferState source = buffer.state();
        AIKO_ASSERT(source.queueFamily == VK_QUEUE_FAMILY_IGNORED || source.queueFamily == queueFamily, "Compute buffer upload must be recorded by the owning queue");

        const VulkanBufferState transferState =
        {
            .stage = VK_PIPELINE_STAGE_TRANSFER_BIT,
            .access = VK_ACCESS_TRANSFER_WRITE_BIT,
            .queueFamily = queueFamily,
        };

        transitionBuffer(commandBuffer, buffer, transferState);

        const uint32_t frame = m_context.currentFrameIndex();

        vector<VulkanComputeBufferImpl::PendingUpload> uploads = buffer.takePendingUploads();

        for (const auto& upload : uploads)
        {
            AIKO_ASSERT(upload.data.empty() == false, "Compute buffer upload is empty");

            const VkDeviceSize size = static_cast<VkDeviceSize>(upload.data.size());

            AIKO_ASSERT(size % 4 == 0, "Compute upload size must be 4-byte aligned");
            AIKO_ASSERT(upload.offset % 4 == 0, "Compute upload destination offset must be 4-byte aligned");

            const UploadSlice slice = allocateUploadSlice(frame, size, 4);

            std::memcpy(slice.mapped, upload.data.data(), upload.data.size());

            const VkBufferCopy copy =
            {
                .srcOffset = slice.offset,
                .dstOffset = upload.offset,
                .size = size,
            };

            vkCmdCopyBuffer(commandBuffer, slice.buffer, buffer.buffer(), 1, &copy);
        }
    }

    VulkanRenderDevice::UploadSlice VulkanRenderDevice::allocateUploadSlice(uint32_t frameIndex, VkDeviceSize size, VkDeviceSize alignment)
    {
        AIKO_ASSERT(frameIndex < FramesInFlight, "Invalid Vulkan frame index");
        AIKO_ASSERT(size > 0, "Cannot allocate empty Vulkan upload slice");
        AIKO_ASSERT(alignment > 0, "Invalid Vulkan upload alignment");
        AIKO_ASSERT((alignment & (alignment - 1)) == 0, "Vulkan upload alignment must be a power of two");

        auto alignUp = [](VkDeviceSize value, VkDeviceSize align)
            {
                return (value + align - 1) & ~(align - 1);
            };

        vector<UploadArenaChunk>& chunks = m_uploadArenaChunks[frameIndex];

        for (UploadArenaChunk& chunk : chunks)
        {
            const VkDeviceSize alignedOffset = alignUp(chunk.offset, alignment);

            if (alignedOffset > chunk.capacity || size > chunk.capacity - alignedOffset)
            {
                continue;
            }

            const UploadSlice slice
            {
                .buffer = chunk.buffer,
                .offset = alignedOffset,
                .mapped = static_cast<uint8_t*>(chunk.mapped) + alignedOffset,
            };

            chunk.offset = alignedOffset + size;

            return slice;
        }

        VkDeviceSize capacity = DefaultUploadArenaChunkSize;

        while (capacity < size)
        {
            AIKO_ASSERT(capacity <= std::numeric_limits<VkDeviceSize>::max() / 2, "Vulkan upload arena size overflow");
            capacity *= 2;
        }

        UploadArenaChunk chunk{};

        const VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        const VkMemoryPropertyFlags propertiesFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if (m_context.hasDedicatedComputeQueue())
        {
            const std::array<uint32_t, 2> queueFamilies =
            {
                m_context.graphicsQueueFamily(),
                m_context.computeQueueFamily(),
            };

            m_context.createBuffer(capacity, usageFlags, propertiesFlags, chunk.buffer, chunk.memory, VK_SHARING_MODE_CONCURRENT, queueFamilies.data(), static_cast<uint32_t>(queueFamilies.size()));
        }
        else
        {
            m_context.createBuffer(capacity, usageFlags, propertiesFlags, chunk.buffer, chunk.memory);
        }

        const VkResult mapResult = vkMapMemory(m_context.device(), chunk.memory, 0, capacity, 0, &chunk.mapped);
        AIKO_ASSERT(mapResult == VK_SUCCESS, "Failed to map Vulkan upload arena");

        chunk.capacity = capacity;
        chunk.offset = size;

        const UploadSlice slice
        {
            .buffer = chunk.buffer,
            .offset = 0,
            .mapped = chunk.mapped,
        };

        chunks.push_back(chunk);

        return slice;
    }

    void VulkanRenderDevice::destroyUploadArena()
    {
        VkDevice device = m_context.device();

        for (uint32_t frame = 0; frame < FramesInFlight; ++frame)
        {
            for (UploadArenaChunk& chunk : m_uploadArenaChunks[frame])
            {
                if (chunk.mapped != nullptr)
                {
                    vkUnmapMemory(device, chunk.memory);
                    chunk.mapped = nullptr;
                }

                if (chunk.buffer != VK_NULL_HANDLE)
                {
                    vkDestroyBuffer(device, chunk.buffer, nullptr);
                    chunk.buffer = VK_NULL_HANDLE;
                }

                if (chunk.memory != VK_NULL_HANDLE)
                {
                    vkFreeMemory(device, chunk.memory, nullptr);
                    chunk.memory = VK_NULL_HANDLE;
                }
                chunk.capacity = 0;
                chunk.offset = 0;
            }
            m_uploadArenaChunks[frame].clear();
        }
    }

    void VulkanRenderDevice::resetUploadArenaForFrame(uint32_t frameIndex)
    {
        AIKO_ASSERT(frameIndex < FramesInFlight, "Invalid Vulkan frame index");
        for (UploadArenaChunk& chunk : m_uploadArenaChunks[frameIndex])
        {
            chunk.offset = 0;
        }
    }

    void VulkanRenderDevice::recordReadbackCopies()
    {
        if (m_readbackRequests.empty())
        {
            return;
        }

        VkCommandBuffer commandBuffer = m_context.activeCommandBuffer();

        AIKO_ASSERT(commandBuffer != VK_NULL_HANDLE,"Readback requires an active frame command buffer");

        const uint32_t frame = m_context.currentFrameIndex();

        for (const ReadbackRequest& request : m_readbackRequests)
        {
            auto* bufferImpl = static_cast<VulkanComputeBufferImpl*>(request.buffer->getImpl());
            AIKO_ASSERT(bufferImpl != nullptr && bufferImpl->isValid(), "Invalid compute readback source buffer");

            AIKO_ASSERT(hasFlag(bufferImpl->usage(),ComputeBufferUsage::TransferSrc), "Compute readback requires transfer-source usage");

            VkBuffer stagingBuffer = VK_NULL_HANDLE;
            VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

            m_context.createBuffer(request.byteSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);

            const VulkanBufferState readbackState =
            {
                .stage = VK_PIPELINE_STAGE_TRANSFER_BIT,
                .access = VK_ACCESS_TRANSFER_READ_BIT,
                .queueFamily = m_context.graphicsQueueFamily(),
            };

            prepareBufferForGraphics(*bufferImpl, readbackState);

            const VkBufferCopy copy =
            {
                .srcOffset = 0,
                .dstOffset = 0,
                .size = request.byteSize,
            };

            vkCmdCopyBuffer(commandBuffer, bufferImpl->buffer(), stagingBuffer, 1, &copy);

            const VkBufferMemoryBarrier hostBarrier =
            {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_HOST_READ_BIT,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = stagingBuffer,
                .offset = 0,
                .size = request.byteSize,
            };

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 0, nullptr, 1, &hostBarrier, 0, nullptr);

            m_inFlightReadbacks.push_back(
            {
                .id = request.id,
                .stagingBuffer = stagingBuffer,
                .stagingMemory = stagingMemory,
                .byteSize = request.byteSize,
                .frameIndex = frame,
            });
        }

        m_readbackRequests.clear();
    }

    void VulkanRenderDevice::completeReadbacksForFrame(uint32_t frameIndex)
    {
        VkDevice device = m_context.device();

        auto it = m_inFlightReadbacks.begin();

        while (it != m_inFlightReadbacks.end())
        {
            if (it->frameIndex != frameIndex)
            {
                ++it;
                continue;
            }

            CompletedReadback completed{};
            completed.id = it->id;
            completed.data.resize(it->byteSize);

            void* mapped = nullptr;

            const VkResult result = vkMapMemory(device, it->stagingMemory, 0, it->byteSize, 0, &mapped);
            AIKO_ASSERT(result == VK_SUCCESS, "Failed to map Vulkan readback staging buffer");

            std::memcpy(completed.data.data(), mapped, it->byteSize);

            vkUnmapMemory(device, it->stagingMemory);
            vkDestroyBuffer(device, it->stagingBuffer, nullptr);
            vkFreeMemory(device, it->stagingMemory, nullptr);

            m_completedReadbacks.push_back(std::move(completed));

            it = m_inFlightReadbacks.erase(it);
        }
    }

    void VulkanRenderDevice::destroyReadbackResources()
    {
        for (InFlightReadback& readback : m_inFlightReadbacks)
        {
            if (readback.stagingBuffer != VK_NULL_HANDLE)
            {
                vkDestroyBuffer(m_context.device(), readback.stagingBuffer, nullptr);
            }

            if (readback.stagingMemory != VK_NULL_HANDLE)
            {
                vkFreeMemory(m_context.device(), readback.stagingMemory, nullptr);
            }
        }

        m_inFlightReadbacks.clear();
        m_readbackRequests.clear();
        m_completedReadbacks.clear();
    }

    void VulkanRenderDevice::createGpuReadResources()
    {

        std::array<VkDescriptorSetLayoutBinding, abi::MaxGpuReadBindings> bindings{};

        for (uint32_t i = 0; i < abi::MaxGpuReadBindings; ++i)
        {
            bindings[i] =
            {
                .binding = i,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            };
        }

        const VkDescriptorSetLayoutCreateInfo layoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindings.size()),
            .pBindings = bindings.data(),
        };

        const VkResult layoutResult = vkCreateDescriptorSetLayout(m_context.device(), &layoutInfo, nullptr, &m_gpuReadDescriptorSetLayout);
        AIKO_ASSERT( layoutResult == VK_SUCCESS, "Failed to create GPU-read descriptor layout");

        for (uint32_t frame = 0; frame < FramesInFlight; ++frame)
        {
            const VkDescriptorPoolSize poolSize =
            {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = abi::MaxGpuReadBindings * MaxGpuDrawsPerFrame,
            };

            const VkDescriptorPoolCreateInfo poolInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .maxSets = MaxGpuDrawsPerFrame,
                .poolSizeCount = 1,
                .pPoolSizes = &poolSize,
            };

            const VkResult poolResult = vkCreateDescriptorPool(m_context.device(), &poolInfo, nullptr, &m_gpuReadDescriptorPools[frame]);

            AIKO_ASSERT(poolResult == VK_SUCCESS, "Failed to create GPU-read descriptor pool");
        }
    }

    void VulkanRenderDevice::destroyGpuReadResources()
    {
        VkDevice device = m_context.device();
        for (VkDescriptorPool& pool : m_gpuReadDescriptorPools)
        {
            if (pool != VK_NULL_HANDLE)
            {
                vkDestroyDescriptorPool(device, pool, nullptr);
                pool = VK_NULL_HANDLE;
            }
        }

        if (m_gpuReadDescriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(device, m_gpuReadDescriptorSetLayout, nullptr);
            m_gpuReadDescriptorSetLayout = VK_NULL_HANDLE;
        }
    }

    VkDescriptorSet VulkanRenderDevice::allocateGpuReadDescriptorSet()
    {

        const uint32_t frame = m_context.currentFrameIndex();
        AIKO_ASSERT(frame < FramesInFlight, "Invalid Vulkan frame index");

        const VkDescriptorSetAllocateInfo info =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = m_gpuReadDescriptorPools[frame],
            .descriptorSetCount = 1,
            .pSetLayouts = &m_gpuReadDescriptorSetLayout,
        };

        VkDescriptorSet set = VK_NULL_HANDLE;

        const VkResult result = vkAllocateDescriptorSets(m_context.device(), &info, &set);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to allocate GPU-read descriptor set");

        return set;
    }

    void VulkanRenderDevice::prepareGpuReadBuffers(const vector<GpuReadBufferBinding>& bindings)
    {
        VkCommandBuffer commandBuffer =
        m_context.activeCommandBuffer();

        for (const GpuReadBufferBinding& binding : bindings)
        {
            AIKO_ASSERT(binding.buffer != nullptr, "GPU-read buffer is null");
            AIKO_ASSERT(binding.buffer->isValid(), "GPU-read buffer is invalid");

            auto* bufferImpl = static_cast<VulkanComputeBufferImpl*>(binding.buffer->getImpl());
            AIKO_ASSERT(bufferImpl != nullptr, "Invalid Vulkan GPU-read buffer");

            const VulkanBufferState destination =
            {
                .stage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
                .access = VK_ACCESS_SHADER_READ_BIT,
                .queueFamily = m_context.graphicsQueueFamily(),
            };

            prepareBufferForGraphics(*bufferImpl, destination);
        }
    }

    VkDescriptorSet VulkanRenderDevice::buildGpuReadDescriptorSet(const vector<GpuReadBufferBinding>& bindings)
    {
        AIKO_ASSERT(bindings.size() <= abi::MaxGpuReadBindings, "Too many GPU-read buffers");

        VkDescriptorSet set = allocateGpuReadDescriptorSet();

        std::array<VkDescriptorBufferInfo, abi::MaxGpuReadBindings> infos{};
        std::array<VkWriteDescriptorSet, abi::MaxGpuReadBindings> writes{};

        uint32_t writeCount = 0;

        std::array<bool, abi::MaxGpuReadBindings> used{};

        for (const GpuReadBufferBinding& binding : bindings)
        {
            AIKO_ASSERT(binding.slot < abi::MaxGpuReadBindings, "GPU-read binding exceeds limit");
            AIKO_ASSERT(used[binding.slot] == false, "Duplicate GPU-read binding");

            auto* impl = static_cast<VulkanComputeBufferImpl*>(binding.buffer->getImpl());
            AIKO_ASSERT(impl != nullptr, "Invalid Vulkan GPU-read buffer");
            AIKO_ASSERT(hasFlag(impl->usage(), ComputeBufferUsage::Storage), "GPU shader read requires storage-buffer usage");

            used[binding.slot] = true;

            infos[writeCount] =
            {
                .buffer = impl->buffer(),
                .offset = 0,
                .range = impl->size(),
            };

            writes[writeCount] =
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = set,
                .dstBinding = binding.slot,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pBufferInfo = &infos[writeCount],
            };

            ++writeCount;
        }

        vkUpdateDescriptorSets(m_context.device(), writeCount, writes.data(), 0, nullptr);

        return set;
    }

    VkPipeline VulkanRenderDevice::getOrCreateGpuInstancedPipeline(const Material& material, VkRenderPass renderPass)
    {
        AIKO_ASSERT(material.m_shaderId != InvalidAssetId, "GPU-instanced material has no shader");
        AIKO_ASSERT(renderPass != VK_NULL_HANDLE, "GPU-instanced render pass is invalid");

        Shader& shader = getResources()->getShader(material.m_shaderId);
        AIKO_ASSERT(shader.isValid(), "GPU-instanced shader is invalid");

        auto* shaderImpl = static_cast<VulkanShaderImpl*>(shader.getImpl());
        AIKO_ASSERT(shaderImpl != nullptr, "Invalid Vulkan GPU-instanced shader implementation");

        const GpuPipelineKey key =
        {
            .shaderId = shader.id(),
            .renderPass = renderPass,
        };

        if (const auto it = m_gpuInstancedPipelines.find(key); it != m_gpuInstancedPipelines.end())
        {
            return it->second;
        }

        validateModelShaderAbi(shaderImpl->reflection());
        validateModelPushConstants(shaderImpl->reflection());

        const VkPipelineShaderStageCreateInfo vertShaderStageInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = shaderImpl->vertexModule(),
            .pName = "main",
        };

        const VkPipelineShaderStageCreateInfo fragShaderStageInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = shaderImpl->fragmentModule(),
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

        VkPipeline pipeline = VK_NULL_HANDLE;

        const VkResult result = vkCreateGraphicsPipelines(m_context.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create GPU-instanced graphics pipeline");

        m_gpuInstancedPipelines.emplace(key, pipeline);

        return pipeline;
    }

    void VulkanRenderDevice::destroyGpuInstancedPipelines()
    {

        VkDevice device = m_context.device();

        for (auto& [key, pipeline] : m_gpuInstancedPipelines)
        {
            AIKO_UNUSED(key);

            if (pipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(device, pipeline, nullptr);
            }
        }

        m_gpuInstancedPipelines.clear();
    }

    VkPipeline VulkanRenderDevice::getOrCreateGpuVertexPipeline(const Material& material, VkRenderPass renderPass, VkPrimitiveTopology topology)
    {
        AIKO_ASSERT(material.m_shaderId != InvalidAssetId, "GPU vertex material has no shader");
        AIKO_ASSERT(renderPass != VK_NULL_HANDLE, "GPU vertex render pass is invalid");

        Shader& shader =getResources()->getShader(material.m_shaderId);
        AIKO_ASSERT(shader.isValid(), "GPU vertex shader is invalid");

        auto* shaderImpl = static_cast<VulkanShaderImpl*>(shader.getImpl());
        AIKO_ASSERT(shaderImpl != nullptr, "Invalid Vulkan GPU vertex shader implementation");

        const GpuVertexPipelineKey key =
        {
            .shaderId = shader.id(),
            .renderPass = renderPass,
            .topology = topology,
        };

        if (const auto it = m_gpuVertexPipelines.find(key); it != m_gpuVertexPipelines.end())
        {
            return it->second;
        }

        validateModelShaderAbi(shaderImpl->reflection());
        validateModelPushConstants(shaderImpl->reflection());

        const VkPipelineShaderStageCreateInfo  vertShaderStageInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = shaderImpl->vertexModule(),
            .pName = "main",
        };

        const VkPipelineShaderStageCreateInfo fragShaderStageInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = shaderImpl->fragmentModule(),
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

        const VkVertexInputAttributeDescription  attributeDescription =
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

        const VkPipelineColorBlendStateCreateInfo
            colorBlending =
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
            .layout = m_modelPipelineLayout,
            .renderPass = renderPass,
            .subpass = 0,
        };

        VkPipeline pipeline = VK_NULL_HANDLE;

        const VkResult result = vkCreateGraphicsPipelines(m_context.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create GPU vertex graphics pipeline");

        m_gpuVertexPipelines.emplace(key, pipeline);

        return pipeline;
    }

    void VulkanRenderDevice::destroyGpuVertexPipelines()
    {
        VkDevice device = m_context.device();
        for (auto& [key, pipeline] : m_gpuVertexPipelines)
        {
            AIKO_UNUSED(key);
            if (pipeline != VK_NULL_HANDLE)
            {
                vkDestroyPipeline(device, pipeline, nullptr);
            }
        }
        m_gpuVertexPipelines.clear();
    }

    void VulkanRenderDevice::prepareBufferForGraphics(VulkanComputeBufferImpl& buffer, const VulkanBufferState& destination)
    {
        AIKO_ASSERT(destination.queueFamily == m_context.graphicsQueueFamily(), "Graphics buffer destination must use the graphics queue family");

        VkCommandBuffer graphicsCommandBuffer = m_context.activeCommandBuffer();
        AIKO_ASSERT(graphicsCommandBuffer != VK_NULL_HANDLE, "Graphics buffer preparation requires an active graphics command buffer");

        const VulkanBufferState source = buffer.state();

        if (buffer.hasPendingUploads())
        {
            if (m_context.hasDedicatedComputeQueue() && source.queueFamily == m_context.computeQueueFamily())
            {

                VkCommandBuffer computeCommandBuffer = m_context.computeCommandBuffer();

                const VulkanBufferState transferDestination =
                {
                    .stage = VK_PIPELINE_STAGE_TRANSFER_BIT,
                    .access = VK_ACCESS_TRANSFER_WRITE_BIT,
                    .queueFamily = m_context.graphicsQueueFamily(),
                };

                releaseBufferOwnership(computeCommandBuffer, buffer, transferDestination);
                acquireBufferOwnership(graphicsCommandBuffer, buffer, source, transferDestination);
            }
            else if (source.queueFamily == VK_QUEUE_FAMILY_IGNORED)
            {
                buffer.setState(
                {
                    .stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                    .access = 0,
                    .queueFamily = m_context.graphicsQueueFamily(),
                });
            }
            else
            {
                AIKO_ASSERT(source.queueFamily == m_context.graphicsQueueFamily(), "Pending buffer upload requires graphics ownership");
            }

            flushComputeBufferUploads(graphicsCommandBuffer, buffer, m_context.graphicsQueueFamily());

        }

        const VulkanBufferState current = buffer.state();

        if (current.queueFamily == VK_QUEUE_FAMILY_IGNORED)
        {
            buffer.setState(destination);
            return;
        }

        if (current.queueFamily == m_context.graphicsQueueFamily())
        {
            transitionBuffer(graphicsCommandBuffer, buffer, destination);
            return;
        }

        AIKO_ASSERT(m_context.hasDedicatedComputeQueue(), "Buffer is compute-owned without a dedicated compute queue");
        AIKO_ASSERT(current.queueFamily == m_context.computeQueueFamily(), "Graphics buffer has unexpected queue ownership");

        VkCommandBuffer computeCommandBuffer = m_context.computeCommandBuffer();
        AIKO_ASSERT(computeCommandBuffer != VK_NULL_HANDLE, "Graphics buffer preparation requires a compute command buffer");

        releaseBufferOwnership(computeCommandBuffer, buffer, destination);
        acquireBufferOwnership(graphicsCommandBuffer, buffer, current, destination);

    }

    void VulkanRenderDevice::waitIdle()
    {
        m_context.waitIdle();
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
