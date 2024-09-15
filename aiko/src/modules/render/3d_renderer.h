#pragma once

#include "modules/render/renderer_context.h"

#include "aiko_types.h"
#include "shared/math.h"
#include "types/color.h"

namespace aiko
{
    class RenderModule;
    class Camera;
    class RenderContext3D : public RenderContext
    {
    public:
        RenderContext3D(RenderModule* renderer) : RenderContext(renderer) { };
        virtual ~RenderContext3D() = default;

    };
}
