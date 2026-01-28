#pragma once

#include <events/event.hpp>
#include <aiko_types.h>

#include "core/singleton.h"
#include "models/shader.h"
#include "models/texture.h"
#include "models/frame_buffer.h"
#include "renderer/Irenderer.h"
#include "types/color.h"

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

    public:

        void setBackgroundColor(const Color);

        // Render models
        void render(const Camera*, const Transform*, const Mesh*, const Shader*);
        void render(const Camera*, const Transform*, const Mesh*, const Shader*, const Texture*);
        void render(const Camera*, const Transform*, const Model*);
        void render(const Camera*, const Transform*, const Mesh*, const Shader*, const FrameBuffer);

        // Font
        void drawText(string, float, float, float = 1.0f, Color = WHITE);

        FrameBuffer getTargetTexture() const;

    protected:

        void onWindowResize(Event&);

        AikoPtr<interfaces::IAikoRenderer> m_renderer;

    };

}
