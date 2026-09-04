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

        VkDescriptorPool m_frameDescriptorPool = VK_NULL_HANDLE;
        std::array<VkBuffer, FramesInFlight> m_frameUniformBuffers{};
        std::array<VkDeviceMemory, FramesInFlight> m_frameUniformMemories{};
        std::array<void*, FramesInFlight> m_frameUniformMapped{};
        std::array<VkDescriptorSet, FramesInFlight> m_frameDescriptorSets{};

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

        struct CachedMaterialBinding
        {
            VkBuffer uniformBuffer = VK_NULL_HANDLE;
            VkDeviceMemory uniformMemory = VK_NULL_HANDLE;
            void* uniformMapped = nullptr;
            VkDeviceSize uniformSize = 0;

            VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

            VkImageView imageView = VK_NULL_HANDLE;
            VkSampler sampler = VK_NULL_HANDLE;

            bool hasTexture = false;
        };

        Texture m_whiteTexture;
        std::array<VkDescriptorPool, FramesInFlight> m_materialDescriptorPools{};

        struct MaterialBindingKey
        {
            AssetId shaderId = InvalidAssetId;
            AssetId diffuseTextureId = InvalidAssetId;
            const Texture* runtimeDiffuseTexture = nullptr;

            std::vector<uint8_t> uniformData{};

            bool operator==(const MaterialBindingKey& other) const
            {
                return shaderId == other.shaderId
                    && diffuseTextureId == other.diffuseTextureId
                    && runtimeDiffuseTexture == other.runtimeDiffuseTexture
                    && uniformData == other.uniformData;
            }
        };

        struct MaterialBindingKeyHash
        {
            size_t operator()(const MaterialBindingKey& key) const
            {
                size_t seed = 0;
                utils::hashCombine(std::hash<AssetId>{}(key.shaderId), seed);
                utils::hashCombine(std::hash<AssetId>{}(key.diffuseTextureId), seed);
                utils::hashCombine(std::hash<const Texture*>{}(key.runtimeDiffuseTexture), seed);
                for (const uint8_t value : key.uniformData)
                {
                    utils::hashCombine(std::hash<uint8_t>{}(value), seed);
                }
                return seed;
            }
        };

        struct RenderPassCompatibilityKey
        {
            VkFormat colorFormat = VK_FORMAT_UNDEFINED;
            VkFormat depthFormat = VK_FORMAT_UNDEFINED;
            VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

            bool operator==(const RenderPassCompatibilityKey& other) const
            {
                return colorFormat == other.colorFormat
                    && depthFormat == other.depthFormat
                    && samples == other.samples;
            }
        };

        struct RenderPassCompatibilityKeyHash
        {
            size_t operator()(const RenderPassCompatibilityKey& key) const
            {
                size_t seed = 0;
                utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.colorFormat)), seed);
                utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.depthFormat)), seed);
                utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.samples)), seed);
                return seed;
            }
        };

        struct ModelPipelineKey
        {
            RenderPassCompatibilityKey renderPass{};
            VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            AssetId shaderId = InvalidAssetId;
            FillMode fillMode = FillMode::Solid;

            CullMode cullMode = CullMode::None;

            bool depthTest = true;
            bool depthWrite = true;
            DepthCompare depthCompare = DepthCompare::LessEqual;

            bool blend = false;

            bool instanced = false;

            bool operator==(const ModelPipelineKey& other) const
            {
                return
                    renderPass == other.renderPass &&
                    topology == other.topology &&
                    shaderId == other.shaderId &&
                    fillMode == other.fillMode &&
                    cullMode == other.cullMode &&
                    depthTest == other.depthTest &&
                    depthWrite == other.depthWrite &&
                    depthCompare == other.depthCompare &&
                    blend == other.blend &&
                    instanced == other.instanced;
            }
        };

        struct ModelPipelineKeyHash
        {
            size_t operator()(const ModelPipelineKey& key) const
            {
                size_t seed = 0;
                utils::hashCombine(RenderPassCompatibilityKeyHash{}(key.renderPass), seed);
                utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.topology)), seed);
                utils::hashCombine(std::hash<AssetId>{}(key.shaderId), seed);
                utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(key.fillMode)), seed);
                utils::hashCombine(std::hash<uint32_t>{}( static_cast<uint32_t>(key.cullMode)), seed);
                utils::hashCombine(std::hash<bool>{}(key.depthTest), seed);
                utils::hashCombine(std::hash<bool>{}(key.depthWrite), seed);
                utils::hashCombine(std::hash<uint32_t>{}( static_cast<uint32_t>(key.depthCompare)), seed);
                utils::hashCombine(std::hash<bool>{}(key.blend), seed);
                utils::hashCombine(std::hash<bool>{}(key.instanced), seed);
                return seed;
            }
        };

        RenderPassCompatibilityKey m_activeRenderPassCompatibility{};
        std::unordered_map<ModelPipelineKey, VkPipeline, ModelPipelineKeyHash> m_modelPipelines;

        VkPipeline getOrCreateModelPipeline(VkRenderPass renderPass, VkPrimitiveTopology topology, AssetId shaderId, const RenderState& renderState, bool instanced);

        std::array<std::unordered_map<MaterialBindingKey, CachedMaterialBinding, MaterialBindingKeyHash>, FramesInFlight> m_materialBindingCaches;
        MaterialBindingKey makeMaterialBindingKey(const Material& material, std::vector<uint8_t> uniformData) const;

        void createMaterialResources();
        void destroyMaterialResources();
        void clearMaterialBindings(u32 frame);
        CachedMaterialBinding& resolveMaterialBinding(const Material& material);
        void refreshMaterialTextureBinding(CachedMaterialBinding& binding, const Material& material);

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

        const Texture* resolveMaterialTexture(const Material& material);

        struct ReadbackRequest
        {
            ReadbackId id = InvalidReadbackId;
            AikoPtr<interfaces::IComputeBufferImpl> source;
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

        struct UploadArenaChunk
        {
            VkBuffer buffer = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;

            void* mapped = nullptr;

            VkDeviceSize capacity = 0;
            VkDeviceSize offset = 0;
        };

        struct UploadSlice
        {
            VkBuffer buffer = VK_NULL_HANDLE;
            VkDeviceSize offset = 0;
            void* mapped = nullptr;
        };

        static constexpr VkDeviceSize DefaultUploadArenaChunkSize = 4 * 1024 * 1024;
        std::array<vector<UploadArenaChunk>, FramesInFlight> m_uploadArenaChunks;

        void flushComputeBufferUploads(VkCommandBuffer commandBuffer, VulkanComputeBufferImpl& buffer, uint32_t queueFamily);
        UploadSlice allocateUploadSlice(uint32_t frameIndex, VkDeviceSize size, VkDeviceSize alignment);
        void resetUploadArenaForFrame(uint32_t frameIndex);
        void destroyUploadArena();

        vector<ReadbackRequest> m_readbackRequests;
        vector<InFlightReadback> m_inFlightReadbacks;
        std::deque<CompletedReadback> m_completedReadbacks;
        ReadbackId m_nextReadbackId = 1;

        void recordReadbackCopies();
        void completeReadbacksForFrame(uint32_t frameIndex);
        void destroyReadbackResources();

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
            RenderPassCompatibilityKey renderPass{};

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
                utils::hashCombine(std::hash<RenderResourceId>{}(key.shaderId), seed);
                utils::hashCombine(RenderPassCompatibilityKeyHash{}(key.renderPass), seed);
                return seed;
            }
        };

        std::unordered_map<GpuPipelineKey, VkPipeline, GpuPipelineKeyHash> m_gpuInstancedPipelines;

        VkPipeline getOrCreateGpuInstancedPipeline(const Material& material, VkRenderPass renderPass);
        void destroyGpuInstancedPipelines();

        struct GpuVertexPipelineKey
        {
            RenderResourceId shaderId = InvalidRenderResourceId;
            RenderPassCompatibilityKey renderPass{};
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
                utils::hashCombine(RenderPassCompatibilityKeyHash{}(key.renderPass), seed);
                utils::hashCombine(std::hash<uint32_t>{}( static_cast<uint32_t>(key.topology)), seed);
                return seed;
            }
        };

        std::unordered_map<GpuVertexPipelineKey, VkPipeline, GpuVertexPipelineKeyHash> m_gpuVertexPipelines;

        VkPipeline getOrCreateGpuVertexPipeline(const Material& material, VkRenderPass renderPass, VkPrimitiveTopology topology);

        void destroyGpuVertexPipelines();

    };
}
