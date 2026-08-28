#pragma once

#include <events/event.hpp>
#include <aiko_types.h>

#include "assets/types/material_asset.h"
#include "metadata/material_instance.h"

#include "core/singleton.h"
#include "core/transform.h"
#include "display/display_events.hpp"
#include "models/camera.h"
#include "models/shader.h"
#include "models/texture.h"
#include "models/frame_buffer.h"
#include "models/mesh.h"
#include "models/model.h"
#include "models/screen_fbo.h"
#include "renderer/Irenderdevice.h"
#include <types/color.h>
#include <types/render_types.h>

#include "imgui/aiko_imgui.h"
#include "resources/render_resource_manager.h"

#include <unordered_map>
#include <deque>
#include <optional>

#include "core/utils.h"

namespace aiko
{

    class AikoRenderer
    {
    
    public:

        AikoRenderer(IAssetProvider& assets, IAssetRegistry* registry);
        ~AikoRenderer() = default;

        void init();
        void beginFrame();
        void endFrame();
        void dispose();

        void setBackgroundColor(const Color);

        void submit(const AmbientLight& ambient, const vector<LightData>& data);
        void submit(const Transform& transform, const Mesh& mesh, const Material& material);
        void submit(const Mesh& mesh, const Material& material, const void* data, uint32_t instanceCount, uint16_t stride);
        void submit(const Transform& transform, const Mesh& mesh, const MaterialAsset& materialAsset, const MaterialInstance& materialInstance);

        void submitTransient(const Transform& transform, const Material& material, const MeshAsset& meshAsset, TransientTopology topology);

        void enqueueCompute(const ComputePass& pass);
        void requestReadback(const ComputeReadbackRequest& req);
        bool pollReadback(ComputeReadbackResult& out);

        void drawMeshInstancedGpu(const GpuInstanceDrawDesc& desc);
        void drawBillboards(const GpuBillboardDrawDesc& desc);
        void drawVerticesGpu(const GpuVertexDrawDesc& desc);

        void render(const Camera& camera);

        void setDebugTexture(const Texture* texture);   // nullptr disables

        const FrameBuffer& getTargetTexture() const;

        RenderResourceManager& resources() { return m_resources; }

        void waitIdle();

    protected:

        const Texture* m_debugTexture = nullptr;

        void onWindowResize(WindowResizeEvent&);

        RenderResourceManager m_resources;
        AikoPtr<renderer::IRenderDevice> m_renderer;

    protected:

        Color m_background_color;

        ScreenFbo m_screenFbo;

        std::vector<RenderItem> m_queue;
        std::vector<InstanceItem> m_instancedQueue;
        std::vector<uint8_t> m_instanceDataArena;
        std::vector<ComputePass> m_computeQueue;
        std::vector<GpuInstanceDrawDesc> m_gpuInstanceDraws;
        std::vector<GpuBillboardDrawDesc> m_gpuBillboardQueue;
        std::vector<uint8_t> m_mergedInstanceDataArena;

        std::vector<TransientDrawDesc> m_transientQueue;
        std::vector<GpuVertexDrawDesc> m_gpuVertexDraws;

        std::vector<LightData> m_lights;
        AmbientLight m_ambientLight;

    private:

        struct FrameMaterialKey
        {
            AssetId shaderId = InvalidAssetId;
            AssetId diffuseTextureId = InvalidAssetId;
            const Texture* runtimeDiffuseTexture = nullptr;

            bool useVertexColor = false;
            bool lit = false;
            Color baseColor = WHITE;

            bool operator==(const FrameMaterialKey& other) const
            {
                return shaderId == other.shaderId
                    && diffuseTextureId == other.diffuseTextureId
                    && runtimeDiffuseTexture == other.runtimeDiffuseTexture
                    && useVertexColor == other.useVertexColor
                    && lit == other.lit
                    && baseColor.rgba() == other.baseColor.rgba();
            }
        };

        struct FrameMaterialKeyHash
        {
            size_t operator()(const FrameMaterialKey& key) const
            {
                std::size_t seed = 0;
                utils::hashCombine(std::hash<AssetId>{}(key.shaderId), seed);
                utils::hashCombine(std::hash<AssetId>{}(key.diffuseTextureId), seed);
                utils::hashCombine(std::hash<const Texture*>{}(key.runtimeDiffuseTexture), seed);
                utils::hashCombine(std::hash<bool>{}(key.useVertexColor), seed);
                utils::hashCombine(std::hash<bool>{}(key.lit), seed);
                utils::hashCombine(std::hash<u32>{}(key.baseColor.rgba()), seed);
                return seed;
            }
        };

        struct PreparedRenderPacket
        {
            MeshDrawPacket draw;
            u64 materialId = 0;
        };

        struct PreparedInstancedPacket
        {
            InstancedDrawPacket draw;
            MaterialId materialId = 0;
            RenderResourceId meshId = 0;

            size_t mergedDataOffset = 0;
            size_t mergedByteCount = 0;
        };

        struct PreparedTransientPacket
        {
            const TransientDrawDesc* item = nullptr;
            u64 materialId = 0;
        };

        struct PreparedScenePass
        {
            vector<const GpuInstanceDrawDesc*> gpuInstances;
            vector<const GpuBillboardDrawDesc*> gpuBillboards;
            vector<const GpuVertexDrawDesc*> gpuVertices;
            vector<PreparedRenderPacket> opaque;
            vector<PreparedInstancedPacket> instanced;
            vector<PreparedTransientPacket> transient;
        };

        struct TransientCacheKey
        {
            const MeshAsset* meshAsset = nullptr;
            TransientTopology topology = TransientTopology::Triangles;

            bool operator==(const TransientCacheKey& other) const
            {
                return meshAsset == other.meshAsset
                    && topology == other.topology;
            }
        };

        struct TransientCacheKeyHash
        {
            size_t operator()(const TransientCacheKey& key) const
            {
                std::size_t seed = 0;
                utils::hashCombine(std::hash<const MeshAsset*>{}(key.meshAsset), seed);
                utils::hashCombine(std::hash<int>{}(static_cast<int>(key.topology)), seed);
                return seed;
            }
        };

        renderer::FrameData buildSceneFrameData(const Camera& camera) const;
        void executeComputePasses();
        PreparedScenePass buildScenePass();
        void submitScenePass(const renderer::FrameData& frameData, const PreparedScenePass& passData, const ivec2& size);
        void submitPresentPass(const ivec2& size);

        static_assert(COMPUTE_VIEW < SCENE_VIEW, "Compute View MUST be less than Scene View");

        Material& stageMaterial(const MaterialAsset& materialAsset, const MaterialInstance& materialInstance);
        std::deque<Material> m_frameMaterials;
        std::unordered_map<FrameMaterialKey, Material*, FrameMaterialKeyHash> m_frameMaterialCache;

        const TransientGeometry& resolveTransientGeometry(const MeshAsset& meshAsset, TransientTopology topology);
        std::unordered_map<TransientCacheKey, TransientGeometry, TransientCacheKeyHash> m_transientGeometryCache;

        AikoImgui m_imgui;
        IAssetRegistry* m_assetRegistry = nullptr;

        std::optional<ivec2> m_windowResizeRequest = std::nullopt;

    };

}
