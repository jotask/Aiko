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

        void submit(const Transform& transform, const Mesh& mesh, const Material& material);
        void submitInstanced(const Mesh& mesh, const Material& material, std::vector<InstanceData> instance);
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

        struct RenderItem
        {
            const Mesh* mesh = nullptr;
            const Material* material = nullptr;
            mat4 transform = mat4(1.0f);
        };

        struct InstancedItem
        {
            const Mesh* mesh = nullptr;
            const Material* material = nullptr;
            std::vector<InstanceData> instances;
        };

        std::vector<RenderItem> m_queue;
        std::vector<InstancedItem> m_instancedQueue;

    private:

        const ViewId SCENE_VIEW = 0;
        const ViewId SCREEN_VIEW = 1;
        const ViewId IMGUI_VIEW = 2;

        AikoImgui m_imgui;

    };

}
