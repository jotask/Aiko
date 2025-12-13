#pragma once

#include <events/event.hpp>
#include <aiko_types.h>

#include "models/shader.h"
#include "models/texture.h"
#include "models/frame_buffer.h"
#include "renderer/Irenderer.h"
#include "types/color.h"

namespace aiko
{

    class AikoRenderer
    {
    
    public:

        AikoRenderer();
        ~AikoRenderer() = default;

    protected:

        void init();
        void beginFrame();
        void endFrame();
        void dispose();

    public:

        void setBackgroundColor(const Color);

        // Render models
        void render(Camera*, Transform*, Mesh*, Shader*);
        void render(Camera*, Transform*, Mesh*, Shader*, Texture*);
        void render(Camera*, Transform*, Model*);
        void render(Camera*, Transform*, Mesh*, Shader*, FrameBuffer);

        // Font
        void drawText(string, float, float, float = 1.0f, Color = WHITE);

    protected:

        AikoPtr<interfaces::IAikoRenderer> m_renderer;

    };

}
