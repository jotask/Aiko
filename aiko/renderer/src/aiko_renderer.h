#pragma once

#include <events/event.hpp>
#include <aiko_types.h>

#include "core/singleton.h"
#include "core/transform.h"
#include "models/camera.h"
#include "models/shader.h"
#include "models/texture.h"
#include "models/frame_buffer.h"
#include "models/mesh.h"
#include "models/model.h"
#include "models/screen_fbo.h"
#include "renderer/Irenderdevice.h"
#include "types/color.h"
#include "types/render_types.h"

#include "imgui/aiko_imgui.h"

namespace aiko
{

    class AikoRenderer : public Singleton<AikoRenderer>
    {
    
    public:

        AikoRenderer();
        virtual ~AikoRenderer() override = default;

        void init();
        void beginFrame();
        void endFrame();
        void dispose();

        void setBackgroundColor(const Color);

        void submit(const AmbientLight& ambient, const std::vector<LightData>& data);
        void submit(const Transform& transform, const Mesh& mesh, const Material& material);
        void submit(const Mesh& mesh, const Material& material, const void* data, uint32_t instanceCount, uint16_t stride);

        void dispatchCompute(ViewId viewId, const ComputeShader& shader, u32 groupsX, u32 groupsY, u32 groupsZ);

        void executeCompute(const ComputePass& pass);

        void render(const Camera& camera);

        // Font
        void drawText(string, float, float, float = 1.0f, Color = WHITE);

        FrameBuffer getTargetTexture() const;

    protected:

        void onWindowResize(Event&);

        AikoPtr<renderer::IRenderDevice> m_renderer;

    protected:

        Color m_background_color;

        ScreenFbo m_screenFbo;
        Shader m_passThrough;

        // Test
        ComputeShader m_csGradient;

        std::vector<RenderItem> m_queue;
        std::vector<InstanceItem> m_instancedQueue;
        std::vector<LightData> m_lights;
        AmbientLight m_ambientLight;

    private:

        static constexpr ViewId COMPUTE_VIEW = 0;
        static constexpr ViewId SCENE_VIEW = 1;
        static constexpr ViewId SCREEN_VIEW = 2;
        static constexpr ViewId IMGUI_VIEW = 3;

        static_assert(COMPUTE_VIEW < SCENE_VIEW, "Compute View MUST be less than Scene View");

        AikoImgui m_imgui;

    };

}
