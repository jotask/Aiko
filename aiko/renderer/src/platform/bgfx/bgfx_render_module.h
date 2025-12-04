#ifdef AIKO_BGFX

#pragma once

#include <stack>

#include <aiko_types.h>
#include <events/events.hpp>

#include "render_module.h"
#include "types/textures.h"
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

    class BgfxRenderer : public AikoRenderer
    {
    
    public:

        using ViewId = uint16_t;

        BgfxRenderer();
        virtual ~BgfxRenderer();

    protected:

        virtual void init() override;

        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual void dispose() override;
    
    public:

        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*) override;
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*, Texture*) override;
        virtual void renderMesh(Camera*, Transform*, Mesh*, Shader*, AikoPtr<FrameBuffer>) override;
        virtual void renderTransientBuffer(Camera*, Transform*, Shader*, Mesh*) override;
        virtual void renderModel(Camera*, Transform*, Model*) override;

        virtual void clearBackground(Color) override;

        // Font
        virtual void drawText(string, float, float, float = 1.0f, Color = WHITE) override;

    protected:

        virtual void onWindowResize(Event&) override;
    
    private:

        const ViewId m_kViewOffScreen;
        const ViewId m_kViewMain;

        ViewId currentViewId;

    };

}

#endif