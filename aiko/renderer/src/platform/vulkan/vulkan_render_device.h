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

        virtual void resize(u32 width, u32 height, bool vsync) override;

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual void beginPass(ViewId viewId, const PassDescription& pass, const FrameBuffer* frameBuffer = nullptr) override;
        virtual void endPass() override;

        virtual void present() override;

        virtual void renderMesh(ViewId viewId, const mat4 world, const Mesh& mesh, const Material& material) override;

        virtual void bindMaterial(const Material& material) override;
        virtual void drawMesh(ViewId viewId, const mat4& world, const Mesh& mesh, const Material& material) override;

        virtual void presentFrameBufferToScreen(ViewId viewId, const ScreenFbo& fb) override;
        virtual void presentTextureToScreen(ViewId viewId, const ScreenFbo& screen, const Texture& texture) override;

        virtual void drawMeshInstanced(ViewId viewId, const Mesh& mesh, const Material& material, const void* data, u32 instanceCount, u32 instanceStrideBytes) override;

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

        virtual void drawTransient(ViewId viewId, const TransientDrawDesc& desc) override;

        virtual void prepareTextureForSampling(const Texture& texture) override;
        virtual void prepareMaterial(const Material& material) override;

        virtual void prepareGpuReadBuffers(const vector<GpuReadBufferBinding>& bindings) override;

    private:

        static constexpr uint32_t MaxComputeBufferBindings = 4;
        static constexpr uint32_t MaxComputeImageBindings = 8;
        static constexpr uint32_t ComputeFrameBinding = MaxComputeBufferBindings + MaxComputeImageBindings;

        VulkanContext m_context;

        bool m_frameActive = false;
        bool m_renderPassActive = false;
        VkDescriptorSetLayout m_screenDescriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout m_screenPipelineLayout = VK_NULL_HANDLE;
        VkPipeline m_screenPipeline = VK_NULL_HANDLE;

        VkDescriptorPool m_screenDescriptorPool = VK_NULL_HANDLE;
        VkDescriptorSet m_screenDescriptorSet = VK_NULL_HANDLE;
        VkImageView m_screenDescriptorImageView = VK_NULL_HANDLE;
        VkSampler m_screenDescriptorSampler = VK_NULL_HANDLE;

        VkDescriptorSetLayout m_frameDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_materialDescriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout m_modelPipelineLayout = VK_NULL_HANDLE;
        VkPipeline m_modelPipelineTriangles = VK_NULL_HANDLE;
        VkPipeline m_transientPointPipeline = VK_NULL_HANDLE;
        VkPipeline m_transientLinePipeline = VK_NULL_HANDLE;

        VkRenderPass m_activeRenderPass = VK_NULL_HANDLE;
        VkExtent2D m_activeExtent = {};

        static constexpr size_t FramesInFlight = 2;

        VkDescriptorPool m_frameDescriptorPool = VK_NULL_HANDLE;
        std::array<VkBuffer, FramesInFlight> m_frameUniformBuffers{};
        std::array<VkDeviceMemory, FramesInFlight> m_frameUniformMemories{};
        std::array<void*, FramesInFlight> m_frameUniformMapped{};
        std::array<VkDescriptorSet, FramesInFlight> m_frameDescriptorSets{};

        void createFrameResources();
        void destroyFrameResources();
        void createModelPipeline(VkRenderPass renderPass, VkPrimitiveTopology topology, VkPipeline& pipeline);
        void createModelPipelineLayout();
        void destroyModelPipeline();
        void createScreenPipelineLayout();
        void destroyScreenPipeline();
        void createScreenPipeline();
        void createScreenDescriptorPool();
        VkDescriptorSet getScreenDescriptorSet(const Texture& texture);

        static constexpr uint32_t MaxMaterialBindings = 1024;

        struct CachedMaterialBinding
        {
            VkBuffer uniformBuffer = VK_NULL_HANDLE;
            VkDeviceMemory uniformMemory = VK_NULL_HANDLE;
            void* uniformMapped = nullptr;

            VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

            VkImageView imageView = VK_NULL_HANDLE;
            VkSampler sampler = VK_NULL_HANDLE;

            bool hasTexture = false;
        };

        Texture m_whiteTexture;
        VkDescriptorPool m_materialDescriptorPool = VK_NULL_HANDLE;

        struct MaterialBindingKey
        {
            AssetId shaderId = InvalidAssetId;
            AssetId diffuseTextureId = InvalidAssetId;
            const Texture* runtimeDiffuseTexture = nullptr;
            bool useVertexColor = false;
            bool lit = false;
            u32 baseColor = 0;

            bool operator==(const MaterialBindingKey& other) const
            {
                return shaderId == other.shaderId
                    && diffuseTextureId == other.diffuseTextureId
                    && runtimeDiffuseTexture == other.runtimeDiffuseTexture
                    && useVertexColor == other.useVertexColor
                    && lit == other.lit
                    && baseColor == other.baseColor;
            }
        };

        struct MaterialBindingKeyHash
        {
            size_t operator()(const MaterialBindingKey& key) const
            {
                std::size_t seed = 0;
                utils::hashCombine(std::hash<AssetId>{}(key.shaderId), seed);
                utils::hashCombine(std::hash<AssetId>{}(key.diffuseTextureId), seed);
                utils::hashCombine(std::hash<const Texture*>{}(key.runtimeDiffuseTexture), seed);
                utils::hashCombine(std::hash<bool>{}(key.useVertexColor), seed);
                utils::hashCombine(std::hash<bool>{}(key.lit), seed);
                utils::hashCombine(std::hash<u32>{}(key.baseColor), seed);
                return seed;
            }
        };

        std::unordered_map<MaterialBindingKey, CachedMaterialBinding, MaterialBindingKeyHash> m_materialBindingCache;
        MaterialBindingKey makeMaterialBindingKey(const Material& material) const;

        void createMaterialResources();
        void destroyMaterialResources();
        CachedMaterialBinding& resolveMaterialBinding(const Material& material);

        mat4 m_sceneViewProj = mat4(1.0f);

        std::unordered_map<const TransientGeometry*, AikoUPtr<Mesh>> m_transientMeshCache;

        void destroyTransientResources();
        Mesh& resolveTransientMesh(const TransientGeometry& geometry);

        void drawMeshWithPipeline(ViewId viewId, const mat4& world, const Mesh& mesh, VkPipeline pipeline);

        VkDescriptorSetLayout m_computeDescriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout m_computePipelineLayout = VK_NULL_HANDLE;
        void createComputePipelineLayout();
        void destroyComputePipelineLayout();

        std::unordered_map<VkShaderModule, VkPipeline> m_computePipelines;

        VkPipeline getOrCreateComputePipeline(VkShaderModule shaderModule);
        void destroyComputePipelines();

        static constexpr uint32_t MaxComputeDispatchesPerFrame = 256;

        std::array<VkDescriptorPool, FramesInFlight> m_computeDescriptorPools{};

        void createComputeDescriptorPools();
        void destroyComputeDescriptorPools();

        VkDescriptorSet allocateComputeDescriptorSet();

        void updateComputeDescriptors(VkDescriptorSet descriptorSet, const std::vector<ComputeBufferBinding>& bindings, const std::vector<ComputeImageBinding>& images);
        void transitionComputeImages(VkCommandBuffer commandBuffer, const vector<ComputeImageBinding>& bindings);
        void transitionTexture(VkCommandBuffer commandBuffer, VulkanTextureImpl& texture, const VulkanImageState& destination);
        void transitionBuffer(VkCommandBuffer commandBuffer, VulkanComputeBufferImpl& buffer, const VulkanBufferState& destination);
        void transitionComputeBuffers(VkCommandBuffer commandBuffer, const vector<ComputeBufferBinding>& bindings);
        VulkanBufferState computeBufferState(ComputeAccess access) const;

        VulkanImageState computeImageState(ComputeAccess access) const;

        const Texture* resolveMaterialTexture(const Material& material);

        struct ReadbackRequest
        {
            ReadbackId id = InvalidReadbackId;
            const ComputeBuffer* buffer = nullptr;
            uint32_t byteSize = 0;
        };

        struct InFlightReadback
        {
            ReadbackId id = InvalidReadbackId;

            VkBuffer stagingBuffer = VK_NULL_HANDLE;
            VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

            uint32_t byteSize = 0;
            uint32_t frameIndex = 0;
        };

        struct CompletedReadback
        {
            ReadbackId id = InvalidReadbackId;
            vector<uint8_t> data;
        };

        struct UploadStagingResource
        {
            VkBuffer buffer = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
        };

        std::array<vector<UploadStagingResource>, FramesInFlight> m_uploadStagingResources;

        void flushComputeBufferUploads(VkCommandBuffer commandBuffer, VulkanComputeBufferImpl& buffer);
        void destroyUploadResourcesForFrame(uint32_t frameIndex);
        void destroyUploadResources();

        vector<ReadbackRequest> m_readbackRequests;
        vector<InFlightReadback> m_inFlightReadbacks;
        std::deque<CompletedReadback> m_completedReadbacks;
        ReadbackId m_nextReadbackId = 1;

        void recordReadbackCopies();
        void completeReadbacksForFrame(uint32_t frameIndex);
        void destroyReadbackResources();

        static constexpr uint32_t MaxGpuReadBindings = 16;
        static constexpr uint32_t MaxGpuDrawsPerFrame = 256;

        std::array<VkDescriptorPool, FramesInFlight> m_gpuReadDescriptorPools{};
        VkDescriptorSetLayout m_gpuReadDescriptorSetLayout = VK_NULL_HANDLE;

        void createGpuReadResources();
        void destroyGpuReadResources();

        VkDescriptorSet allocateGpuReadDescriptorSet();
        VkDescriptorSet buildGpuReadDescriptorSet(const vector<GpuReadBufferBinding>& bindings);

        struct GpuPipelineKey
        {
            RenderResourceId shaderId = InvalidRenderResourceId;
            VkRenderPass renderPass = VK_NULL_HANDLE;

            bool operator==(const GpuPipelineKey& other) const
            {
                return shaderId == other.shaderId &&renderPass == other.renderPass;
            }
        };

        struct GpuPipelineKeyHash
        {
            size_t operator()(const GpuPipelineKey& key) const
            {
                size_t seed = 0;
                utils::hashCombine( std::hash<RenderResourceId>{}(key.shaderId), seed);
                utils::hashCombine(std::hash<VkRenderPass>{}(key.renderPass), seed);
                return seed;
            }
        };

        std::unordered_map<GpuPipelineKey, VkPipeline, GpuPipelineKeyHash> m_gpuInstancedPipelines;

        VkPipeline getOrCreateGpuInstancedPipeline(const Material& material, VkRenderPass renderPass);
        void destroyGpuInstancedPipelines();

        struct GpuVertexPipelineKey
        {
            RenderResourceId shaderId = InvalidRenderResourceId;
            VkRenderPass renderPass = VK_NULL_HANDLE;
            VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

            bool operator==(const GpuVertexPipelineKey& other) const
            {
                return shaderId == other.shaderId
                    && renderPass == other.renderPass
                    && topology == other.topology;
            }
        };

        struct GpuVertexPipelineKeyHash
        {
            size_t operator()(const GpuVertexPipelineKey& key) const
            {
                size_t seed = 0;
                utils::hashCombine(std::hash<RenderResourceId>{}(key.shaderId), seed);
                utils::hashCombine(std::hash<VkRenderPass>{}(key.renderPass), seed);
                utils::hashCombine(std::hash<uint32_t>{}( static_cast<uint32_t>(key.topology)), seed);
                return seed;
            }
        };

        std::unordered_map<GpuVertexPipelineKey, VkPipeline, GpuVertexPipelineKeyHash> m_gpuVertexPipelines;

        VkPipeline getOrCreateGpuVertexPipeline(const Material& material, VkRenderPass renderPass, VkPrimitiveTopology topology);

        void destroyGpuVertexPipelines();

    };
}
