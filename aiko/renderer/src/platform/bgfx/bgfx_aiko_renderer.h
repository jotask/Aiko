#ifdef AIKO_BGFX

#pragma once

#include <stack>

#include <aiko_types.h>
#include <events/events.hpp>

#include "renderer/Irenderer.h"
#include "models/shader.h"
#include "models/model.h"
#include "models/texture.h"
#include "types/color.h"

namespace aiko
{
    class Camera;
    class Mesh;
    class Shader;
    class Transform;
}

namespace aiko::bgfx
{

    class BgfxRenderer : public interfaces::IAikoRenderer
    {
    
    public:

        using ViewId = uint16_t;

        BgfxRenderer();
        virtual ~BgfxRenderer() override;

    public:

        virtual void init() override;
        virtual void beginFrame() override;
        virtual void endFrame() override;
        virtual void dispose() override;

        virtual void clearBackground(Color) override;

        virtual void render(const Camera*, const Transform*, const Mesh*, const Shader*) override;
        virtual void render(const Camera*, const Transform*, const Mesh*, const Shader*, const Texture*) override;
        virtual void render(const Camera*, const Transform*, const Model*) override;
        virtual void render(const Camera*, const Transform*, const Mesh*, const Shader*, const FrameBuffer) override;

        virtual void renderTransientBuffer(Camera*, Transform*, Shader*, Mesh*) override;

        // Font
        virtual void drawText(string, float, float, float = 1.0f, Color = WHITE) override;

    protected:

        virtual void onWindowResize(Event&) override;
    
    private:

        void initScreenFbo();

        const ViewId m_kViewOffScreen;
        const ViewId m_kViewMain;

        ViewId currentViewId;

    };

}

#endif