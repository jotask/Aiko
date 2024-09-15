#pragma once

#include <vector>

#include "modules/render/3d_renderer.h"
#include "modules/render/render_module.h"
#include "models/shader.h"

namespace aiko
{
    class RenderModule;
    class Opengl3DRenderer : public RenderContext3D
    {
    public:

        Opengl3DRenderer(RenderModule* renderer);
        virtual ~Opengl3DRenderer() = default;

        virtual void init();
        virtual void beginFrame();
        virtual void endFrame();
        virtual void dispose();

    private:

    };
}
