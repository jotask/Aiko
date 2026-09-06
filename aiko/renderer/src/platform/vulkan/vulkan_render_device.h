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
        VkDescriptorSetLayout m_screenDescriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout m_screenPipelineLayout = VK_NULL_HANDLE;
        VkPipeline m_screenPipeline = VK_NULL_HANDLE;

        VkDescriptorPool m_screenDescriptorPool = VK_NULL_HANDLE;
        std::array<VkDescriptorSet, FramesInFlight> m_screenDescriptorSets{};

        VkDescriptorSetLayout m_frameDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_materialDescriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout m_modelPipelineLayout = VK_NULL_HANDLE;

        VkRenderPass m_activeRenderPass = VK_NULL_HANDLE;
        VkExtent2D m_activeExtent = {};
        VulkanTextureImpl* m_activeColorAttachment = nullptr;
        VulkanTextureImpl* m_activeDepthAttachment = nullptr;

        static constexpr uint32_t MaxFrameBindingsPerFrame = 32;

        std::array<VkDescriptorPool, FramesInFlight> m_frameDescriptorPools{};
        std::array<std::vector<VulkanFrameBinding>, FramesInFlight> m_frameBindings;

        void createFrameResources();
        void destroyFrameResources();
        void createModelPipeline(VkRenderPass renderPass, VkPrimitiveTopology topology, AssetId shaderId, const RenderState& renderState, VkPipeline& pipeline);
        void createModelInstancedPipeline(VkRenderPass renderPass, const RenderState& renderState, VkPipeline& pipeline);
        void createModelPipelineLayout();
        void destroyModelPipeline();
        void createScreenPipelineLayout();
        void destroyScreenPipeline();
        void createScreenPipeline();
        void createScreenDescriptorPool();
        VkDescriptorSet getScreenDescriptorSet(const Texture& texture);

        static constexpr uint32_t MaxMaterialBindings = 1024;

        Texture m_whiteTexture;
        std::array<VulkanMaterialFrameResources, FramesInFlight> m_materialFrameResources = {};

        RenderPassCompatibilityKey m_activeRenderPassCompatibility{};
        std::unordered_map<ModelPipelineKey, VkPipeline, ModelPipelineKeyHash> m_modelPipelines;

        VkPipeline getOrCreateModelPipeline(VkRenderPass renderPass, VkPrimitiveTopology topology, AssetId shaderId, const RenderState& renderState, bool instanced);

        MaterialBindingKey makeMaterialBindingKey(const Material& material, std::vector<MaterialTextureBindingKey> textures, std::vector<uint8_t> uniformData) const;

        void createMaterialResources();
        void destroyMaterialResources();
        void resetMaterialFrameResources(u32 frame);
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

        VkDescriptorSetLayout m_computeDescriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout m_computePipelineLayout = VK_NULL_HANDLE;
        void createComputePipelineLayout();
        void destroyComputePipelineLayout();

        std::unordered_map<VkShaderModule, VkPipeline> m_computePipelines;

        VkPipeline getOrCreateComputePipeline(const VulkanComputeShaderImpl& shader);
        void destroyComputePipelines();

        static constexpr uint32_t MaxComputeDispatchesPerFrame = 256;

        std::array<VkDescriptorPool, FramesInFlight> m_computeDescriptorPools{};

        void createComputeDescriptorPools();
        void destroyComputeDescriptorPools();

        VkDescriptorSet allocateComputeDescriptorSet();

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
        vector<InFlightReadback> m_inFlightReadbacks;
        std::deque<CompletedReadback> m_completedReadbacks;
        ReadbackId m_nextReadbackId = 1;

        void recordReadbackCopies();
        void completeReadbacksForFrame(uint32_t frameIndex);
        void destroyReadbackResources();

        VulkanGpuReadDescriptors m_gpuReadDescriptors;

        VkDescriptorSet buildGpuReadDescriptorSet(const vector<GpuReadBufferBinding>& bindings);

        std::unordered_map<GpuPipelineKey, VkPipeline, GpuPipelineKeyHash> m_gpuInstancedPipelines;

        VkPipeline getOrCreateGpuInstancedPipeline(const Material& material, VkRenderPass renderPass);
        void destroyGpuInstancedPipelines();

        std::unordered_map<GpuVertexPipelineKey, VkPipeline, GpuVertexPipelineKeyHash> m_gpuVertexPipelines;

        VkPipeline getOrCreateGpuVertexPipeline(const Material& material, VkRenderPass renderPass, VkPrimitiveTopology topology);

        void destroyGpuVertexPipelines();

        VulkanSamplerCache m_samplerCache;

        const Texture* resolveTextureBinding(const TextureBinding& binding);
        void refreshMaterialTextureBindings(VulkanMaterialBinding& binding, const std::vector<const VulkanShaderDescriptorBinding*>& descriptors, const std::vector<TextureBinding>& textureBindings);

        void resetFrameBindings(u32 frame);

    };
}
