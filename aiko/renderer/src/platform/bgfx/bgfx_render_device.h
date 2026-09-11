#pragma once

#include <deque>
#include <optional>
#include <unordered_set>

#include "aiko_types.h"
#include "bgfx_types.h"
#include "core/utils.h"
#include "renderer/Irenderdevice.h"

namespace aiko::renderer::bgfx
{
    class BgfxTextureImpl;

    class BgfxShaderImpl;

    class BgfxRenderDevice final : public IRenderDevice
    {
    public:

        BgfxRenderDevice(RenderResourceManager*);
        virtual ~BgfxRenderDevice() override;

        virtual bool init(const renderer::DeviceInitDesc& desc) override;
        virtual void shutdown() override;

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

        // --- Compute readback (BGFX internal) ---
        struct PendingReadback
        {
            ReadbackId id = 0;
            const ComputeBuffer* source = nullptr;
            uint32_t byteSize = 0;

            ::bgfx::TextureHandle computeTex = AIKO_INVALID_HANDLE; // compute-write
            ::bgfx::TextureHandle readTex    = AIKO_INVALID_HANDLE; // read-back

            uint32_t framesSinceRequest = 0;

            enum class Stage : uint8_t
            {
                Idle = 0,
                SubmittedGPUWork,
                ReadIssued,
                Done
            };

            Stage stage = Stage::Idle;
            uint32_t framesSinceSubmit = 0;

            vector<uint8_t> cpu;
        };

        struct PreparedFrameUniforms
        {
            vec3 cameraPosition = vec3(0.0f);
            vec4 ambientColor = vec4(0.0f);
            float ambientIntensity = 0.0f;

            uint32_t lightCount = 0;

            vec4 lightType[MAX_LIGHTS] = {};
            vec4 lightPosRange[MAX_LIGHTS] = {};
            vec4 lightDir[MAX_LIGHTS] = {};
            vec4 lightColorInt[MAX_LIGHTS] = {};
            vec4 lightSpotCos[MAX_LIGHTS] = {};
        };

        struct MaterialBindingKey
        {
            AssetId shaderId = InvalidAssetId;
            AssetId diffuseTextureId = InvalidAssetId;
            const Texture* runtimeDiffuseTexture = nullptr;

            bool operator==(const MaterialBindingKey& other) const
            {
                return shaderId == other.shaderId
                    && diffuseTextureId == other.diffuseTextureId
                    && runtimeDiffuseTexture == other.runtimeDiffuseTexture;
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
                return seed;
            }
        };

        struct CachedMaterialBinding
        {
            BgfxShaderImpl* shader = nullptr;
            ::bgfx::UniformHandle textureSampler = AIKO_INVALID_HANDLE;
            BgfxTextureImpl* texture = nullptr;
            bool hasTexture = false;
        };

        void dispatchPendingReadbackCopy();
        void startReadbackInternal(const ComputeReadbackRequest& r);
        void cleanupReadback(PendingReadback& rb);

        std::deque<ComputeReadbackRequest> m_readbackQueue;
        std::deque<ComputeReadbackResult>  m_completedReadbacks;
        std::optional<PendingReadback>     m_activeReadback;

        ReadbackId m_nextReadbackId = 1;

        // Hidden compute shader that copies buffer -> texture
        ComputeShader m_csReadbackCopy;

        u32 m_width;
        u32 m_height;
        bool m_vsync = true;

        FrameData m_frameData;
        PreparedFrameUniforms m_preparedFrameUniforms;
        std::unordered_set<const BgfxShaderImpl*> m_frameUniformsUploaded;

        std::unordered_map<MaterialBindingKey, CachedMaterialBinding, MaterialBindingKeyHash> m_materialBindingCache;
        const CachedMaterialBinding* m_boundMaterialBinding = nullptr;
        const Material* m_lastMaterialUniformSource = nullptr;
        const Material* m_lastBoundMaterialSource = nullptr;

        BgfxShaderImpl* m_boundShader;

        MaterialBindingKey makeMaterialBindingKey(const Material& material) const;
        const CachedMaterialBinding& resolveMaterialBinding(const Material& material);

        void prepareFrameUniforms();
        void bindFrameUniforms();
        void bindMaterialUniforms(const Material& material);

    };
}
