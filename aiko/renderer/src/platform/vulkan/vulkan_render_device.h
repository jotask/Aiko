#pragma once

#include <deque>
#include <optional>
#include <unordered_set>
#include <unordered_map>

#include "aiko_types.h"
#include "vulkan_context.h"
#include "vulkan_types.h"
#include "core/utils.h"
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

        virtual bool init(const renderer::DeviceInitDesc& desc) override;
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

        virtual void drawTransient(ViewId viewId, const TransientDrawDesc& desc) override;

    private:

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

    };
}
