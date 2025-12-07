#pragma once

#include <events/event.hpp>
#include <aiko_types.h>
#include <core/singleton.h>

#include "models/shader.h"
#include "models/model.h"
#include "models/texture.h"
#include "models/frame_buffer.h"
#include "models/screen_fbo.h"

#include "types/color.h"

namespace aiko
{

    class Camera;
    class DisplayModule;
    class Mesh;
    class Shader;
    class Transform;

    class AikoRenderer
    {
    
    public:

        AikoRenderer() = default;
        virtual ~AikoRenderer() = default;

    protected:

        virtual void init() = 0;
        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;
        virtual void dispose() = 0;

    public:

        void setBackgroundColor(const Color color);
        virtual void clearBackground(Color) = 0;

        void initScreenFbo();
        virtual AikoPtr<ScreenFbo> getScreenFbo() = 0;

        // Render models
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*) = 0;
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*, Texture*) = 0;
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*, AikoPtr<FrameBuffer>) = 0;
        virtual void renderTransientBuffer(Camera*, Transform*, Shader*, Mesh*) = 0;
        virtual void renderModel(Camera*, Transform*, Model*) = 0;

        // Font
        virtual void drawText(string, float, float, float = 1.0f, Color = WHITE) = 0;

    protected:

        Color m_background_color;
        bool m_scale;

        AikoPtr<ScreenFbo> m_screenFbo;
        Shader m_passThrough;

        virtual void onWindowResize(Event&);
    
    };

}
