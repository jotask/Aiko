#pragma once

#include <aiko_types.h>
#include <events/event.hpp>
#include <core/transform.h>

#include "models/camera.h"
#include "models/shader.h"
#include "models/model.h"
#include "models/texture.h"
#include "models/frame_buffer.h"
#include "models/mesh.h"
#include "models/screen_fbo.h"
#include "types/color.h"

namespace aiko::interfaces
{
    class IAikoRenderer
    {

    public:

        IAikoRenderer() = default;
        virtual ~IAikoRenderer()= default;

        virtual void init() = 0;
        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;
        virtual void dispose() = 0;

        void setBackgroundColor(Color color) { m_background_color = color; };
        virtual void clearBackground(Color) = 0;

        // Render models
        virtual void render(const Camera*, const Transform*, const Mesh*, const Shader*) = 0;
        virtual void render(const Camera*, const Transform*, const Mesh*, const Shader*, const Texture*) = 0;
        virtual void render(const Camera*, const Transform*, const Model*) = 0;
        virtual void render(const Camera*, const Transform*, const Mesh*, const Shader*, const FrameBuffer) = 0;

        virtual void renderTransientBuffer(Camera*, Transform*, Shader*, Mesh*) = 0;

        // Font
        virtual void drawText(string, float, float, float = 1.0f, Color = WHITE) = 0;

        FrameBuffer getFrameBuffer() const
        {
            return m_screenFbo.getFrameBuffer();
        }

    protected:

        virtual void onWindowResize(Event&) = 0;

        Color m_background_color;

        ScreenFbo m_screenFbo;
        Shader m_passThrough;

    };
}


