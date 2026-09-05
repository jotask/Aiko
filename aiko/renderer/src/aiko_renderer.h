#pragma once

#include "assets/types/material_asset.h"
#include "core/transform.h"
#include "display/display_events.hpp"
#include "imgui/aiko_imgui.h"
#include "metadata/material_instance.h"
#include "models/camera.h"
#include "models/mesh.h"
#include "models/model.h"
#include "models/render_target.h"
#include "models/texture.h"
#include "renderer/Irenderdevice.h"
#include "renderer/render_queue.h"
#include "renderer/screen_presenter.h"
#include "resources/render_resource_manager.h"
#include "types/aiko_renderer_types.h"
#include "types/render_config.h"

#include <types/color.h>
#include <types/render_types.h>

#include <aiko_types.h>
#include <deque>
#include <optional>
#include <unordered_map>

namespace aiko
{

    class AikoRenderer
    {
    
    public:

        AikoRenderer(IAssetProvider& assets);
        ~AikoRenderer() = default;

        void init(const RendererConfig& config, const RenderSurfaceDesc& surface);
        void beginFrame();
        void endFrame();
        void dispose();

        void setClearColor(Color);

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

        const RenderTarget& sceneRenderTarget() const { return m_sceneTarget; }

        void setSceneRenderTarget(RenderTarget* target)
        {
            m_sceneRenderTargetOverride = target;
        }

        RenderResourceManager& resources() { return m_resources; }

        void waitIdle();

    protected:

        const Texture* m_debugTexture = nullptr;

        void onWindowResize(WindowResizeEvent&);

        RenderResourceManager m_resources;
        AikoPtr<renderer::IRenderDevice> m_renderer;

    protected:

        Color m_clearColor;
        ivec2 m_renderSurface;

        RenderTarget m_sceneTarget;
        ScreenPresenter m_screenPresenter;

        std::vector<ComputePass> m_computeQueue;

        std::vector<LightData> m_lights;
        AmbientLight m_ambientLight;

    private:

        RenderQueue m_renderQueue;

        renderer::FrameData buildSceneFrameData(const Camera& camera, const ivec2& targetSize) const;
        void executeComputePasses();
        void submitScenePass(const renderer::FrameData& frameData, const PreparedScenePass& passData, const RenderTarget& target);
        void submitPresentPass(const Texture& texture);

        static_assert(COMPUTE_VIEW < SCENE_VIEW, "Compute View MUST be less than Scene View");

        Material& stageMaterial(const MaterialAsset& materialAsset, const MaterialInstance& materialInstance);
        std::deque<Material> m_frameMaterials;
        std::unordered_map<FrameMaterialKey, Material*, FrameMaterialKeyHash> m_frameMaterialCache;

        const TransientGeometry& resolveTransientGeometry(const MeshAsset& meshAsset, TransientTopology topology);
        std::unordered_map<TransientCacheKey, TransientGeometry, TransientCacheKeyHash> m_transientGeometryCache;

        RenderTarget& activeSceneRenderTarget()
        {
            return m_sceneRenderTargetOverride != nullptr ? *m_sceneRenderTargetOverride : m_sceneTarget;
        }

        const RenderTarget& activeSceneRenderTarget() const
        {
            return m_sceneRenderTargetOverride != nullptr ? *m_sceneRenderTargetOverride : m_sceneTarget;
        }

        AikoImgui m_imgui;

        std::optional<ivec2> m_pendingSurfaceResize = std::nullopt;

        RenderTarget* m_sceneRenderTargetOverride = nullptr;

    };

}
