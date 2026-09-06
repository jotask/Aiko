#pragma once

#include <deque>
#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "aiko_types.h"
#include "vulkan_context.h"
#include "vulkan_types.h"
#include "core/utils.h"
#include "impl/vulkan_computebuffer_impl.h"
#include "renderer/Irenderdevice.h"
#include "vulkan_descriptor_abi.h"
#include "impl/vulkan_computeshader_impl.h"

#include "vulkan_upload_arena.h"
#include "vulkan_pipeline_types.h"
#include "vulkan_render_types.h"
#include "vulkan_transfer_types.h"
#include "vulkan_gpu_read_descriptors.h"
#include "vulkan_sampler_cache.h"
#include "vulkan_compute_descriptors.h"
#include "vulkan_compute_pipelines.h"
#include "vulkan_screen_resources.h"
#include "vulkan_frame_resources.h"
#include "vulkan_model_pipelines.h"
#include "vulkan_gpu_pipelines.h"
#include "vulkan_readback_resources.h"
#include "vulkan_material_resources.h"

namespace aiko::renderer::vulkan
{
    class VulkanTextureImpl;
    class VulkanShaderImpl;

    class VulkanRenderDevice final : public IRenderDevice
    {
    public:

        VulkanRenderDevice(RenderResourceManager*);
        virtual ~VulkanRenderDevice() override;

        virtual bool init(const DeviceInitDesc& desc) override;
        virtual void shutdown() override;
        virtual void waitIdle() override;

        virtual void resize(u32 width, u32 height) override;

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual void beginPass(ViewId viewId, const PassDescription& pass, const FrameBuffer* frameBuffer = nullptr) override;
        virtual void endPass() override;

        virtual void present() override;

    protected:

        virtual void bindMaterial(const Material& material) override;
        virtual void drawMesh(ViewId viewId, const mat4& world, const Mesh& mesh, const Material& material) override;
        virtual void drawMeshInstanced(ViewId viewId, const Mesh& mesh, const Material& material, const void* data, u32 instanceCount, u32 instanceStrideBytes) override;
        virtual void drawTransient(ViewId viewId, const TransientDrawDesc& desc) override;

    public:

        virtual void presentTextureToScreen(ViewId viewId, const Mesh& screenMesh, const Texture& texture) override;

        virtual void bindFrame(ViewId viewId, const FrameData& u) override;

        // Compute Shader
        virtual void execute(ViewId viewId, const ComputePass& pass) override;
        virtual void requestReadback(const ComputeReadbackRequest& request) override;
        virtual bool pollReadback(ComputeReadbackResult& result) override;

        virtual void drawMeshInstancedGpu(ViewId viewId, const GpuInstanceDrawDesc& desc) override;
        virtual void drawBillboards(ViewId viewId, const GpuBillboardDrawDesc& desc) override;
        virtual void drawVerticesGpu(ViewId viewId, const GpuVertexDrawDesc& desc) override;
        virtual void prepareVertexBuffer(const ComputeBuffer& buffer) override;
        virtual void prepareIndexBuffer(const ComputeBuffer& buffer) override;
        virtual void prepareIndirectBuffer(const ComputeBuffer& buffer) override;

        virtual void prepareTextureForSampling(const Texture& texture) override;
        virtual void prepareMaterial(const Material& material) override;

        virtual void prepareGpuReadBuffers(const vector<GpuReadBufferBinding>& bindings) override;

    private:

        static constexpr size_t FramesInFlight = 2;

        VulkanContext m_context;

        bool m_frameActive = false;
        bool m_renderPassActive = false;
        bool m_computePassActive = false;

        VkRenderPass m_activeRenderPass = VK_NULL_HANDLE;
        VkExtent2D m_activeExtent = {};
        VulkanTextureImpl* m_activeColorAttachment = nullptr;
        VulkanTextureImpl* m_activeDepthAttachment = nullptr;

        VulkanFrameResources m_frameResources;
        VulkanModelPipelines m_modelPipelines;
        VulkanGpuPipelines m_gpuPipelines;
        VulkanScreenResources m_screenResources;
        VulkanMaterialResources m_materialResources;

        Texture m_whiteTexture;

        RenderPassCompatibilityKey m_activeRenderPassCompatibility{};

        VkPipeline getOrCreateModelPipeline(VkRenderPass renderPass, VkPrimitiveTopology topology, AssetId shaderId, const RenderState& renderState, bool instanced);

        MaterialBindingKey makeMaterialBindingKey(const Material& material, std::vector<MaterialTextureBindingKey> textures, std::vector<uint8_t> uniformData) const;

        VulkanMaterialBinding& resolveMaterialBinding(const Material& material);

        mat4 m_sceneViewProj = mat4(1.0f);

        std::array<std::unordered_map<const TransientGeometry*, AikoUPtr<Mesh>>, FramesInFlight> m_transientMeshCaches{};
        void resetTransientFrameResources(u32 frame);

        void destroyTransientResources();
        Mesh& resolveTransientMesh(const TransientGeometry& geometry);

        AikoUPtr<Mesh> m_billboardMesh;
        AikoUPtr<Mesh> createTransientMesh(const TransientGeometry& geometry);
        Mesh& resolveBillboardMesh();

        void drawMeshWithPipeline(ViewId viewId, const mat4& world, const Mesh& mesh, VkPipeline pipeline);

        VulkanComputeDescriptors m_computeDescriptors;
        VulkanComputePipelines m_computePipelines;

        void updateComputeDescriptors(VkDescriptorSet descriptorSet, const std::vector<ComputeBufferBinding>& bindings, const std::vector<ComputeImageBinding>& images);
        void transitionComputeImages(VkCommandBuffer commandBuffer, const vector<ComputeImageBinding>& bindings, bool useDedicatedCompute);
        VulkanImageState computeImageState(ComputeAccess access, uint32_t queueFamily) const;
        void transitionTexture(VkCommandBuffer commandBuffer, VulkanTextureImpl& texture, const VulkanImageState& destination);
        void transitionBuffer(VkCommandBuffer commandBuffer, VulkanComputeBufferImpl& buffer, const VulkanBufferState& destination);
        void releaseBufferOwnership(VkCommandBuffer commandBuffer, VulkanComputeBufferImpl& buffer, const VulkanBufferState& destination);
        void acquireBufferOwnership(VkCommandBuffer commandBuffer, VulkanComputeBufferImpl& buffer, const VulkanBufferState& source, const VulkanBufferState& destination);
        void prepareBufferForGraphics(VulkanComputeBufferImpl& buffer, const VulkanBufferState& destination);
        void releaseTextureOwnership(VkCommandBuffer commandBuffer, VulkanTextureImpl& texture, const VulkanImageState& destination);
        void acquireTextureOwnership(VkCommandBuffer commandBuffer, VulkanTextureImpl& texture, const VulkanImageState& source, const VulkanImageState& destination);

        void transitionComputeBuffers(VkCommandBuffer commandBuffer, const vector<ComputeBufferBinding>& bindings, bool useDedicatedCompute);
        VulkanBufferState computeBufferState(ComputeAccess access, uint32_t queueFamily) const;

        VulkanUploadArena m_uploadArena;

        void flushComputeBufferUploads(VkCommandBuffer commandBuffer, VulkanComputeBufferImpl& buffer, uint32_t queueFamily);

        vector<ReadbackRequest> m_readbackRequests;
        VulkanReadbackResources m_readbackResources;
        ReadbackId m_nextReadbackId = 1;

        void recordReadbackCopies();
        void destroyReadbackResources();

        VulkanGpuReadDescriptors m_gpuReadDescriptors;

        VkDescriptorSet buildGpuReadDescriptorSet(const vector<GpuReadBufferBinding>& bindings);

        VkPipeline getOrCreateGpuInstancedPipeline(const Material& material, VkRenderPass renderPass);
        VkPipeline getOrCreateGpuVertexPipeline(const Material& material, VkRenderPass renderPass, VkPrimitiveTopology topology);

        VulkanSamplerCache m_samplerCache;

        const Texture* resolveTextureBinding(const TextureBinding& binding);
        void refreshMaterialTextureBindings(VulkanMaterialBinding& binding, const std::vector<const VulkanShaderDescriptorBinding*>& descriptors, const std::vector<TextureBinding>& textureBindings);


    };
}
