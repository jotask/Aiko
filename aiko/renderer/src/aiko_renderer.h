#pragma once

#include <events/event.hpp>
#include <aiko_types.h>

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

namespace aiko
{

    class AikoRenderer
    {
    
    public:

        AikoRenderer(IAssetProvider& assets);
        ~AikoRenderer() = default;

        void init();
        void beginFrame();
        void endFrame();
        void dispose();

        void setBackgroundColor(const Color);

        void submit(const AmbientLight& ambient, const std::vector<LightData>& data);
        void submit(const Transform& transform, const Mesh& mesh, const Material& material);
        void submit(const Mesh& mesh, const Material& material, const void* data, uint32_t instanceCount, uint16_t stride);

        void submitTransient(const Transform& transform, const Material& material, const MeshAsset& meshAsset, TransientTopology topology);

        void enqueueCompute(const ComputePass& pass);
        void requestReadback(const ComputeReadbackRequest& req);
        bool pollReadback(ComputeReadbackResult& out);

        void drawMeshInstancedGpu(const GpuInstanceDrawDesc& desc);
        void drawBillboards(const GpuBillboardDrawDesc& desc);

        void render(const Camera& camera);

        void setDebugTexture(const Texture* texture);   // nullptr disables

        const FrameBuffer& getTargetTexture() const;

        RenderResourceManager& resources() { return m_resources; }

    protected:

        const Texture* m_debugTexture = nullptr;

        void onWindowResize(WindowResizeEvent&);

        AikoPtr<renderer::IRenderDevice> m_renderer;

    protected:

        Color m_background_color;

        ScreenFbo m_screenFbo;
        Shader m_passThrough;

        std::vector<RenderItem> m_queue;
        std::vector<InstanceItem> m_instancedQueue;
        std::vector<ComputePass> m_computeQueue;
        std::vector<GpuInstanceDrawDesc> m_gpuInstanceDraws;
        std::vector<GpuBillboardDrawDesc> m_gpuBillboardQueue;

        std::vector<TransientDrawDesc> m_transientQueue;

        std::vector<LightData> m_lights;
        AmbientLight m_ambientLight;

    private:

        static_assert(COMPUTE_VIEW < SCENE_VIEW, "Compute View MUST be less than Scene View");

        AikoImgui m_imgui;
        RenderResourceManager m_resources;

    };

}
