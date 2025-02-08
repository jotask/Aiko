#pragma once

#include "modules/render/renderer_context.h"

#include "aiko_types.h"
#include "shared/math.h"
#include "types/color.h"

namespace aiko
{
    class RenderModule;
    class Camera;
    class RenderContext2D : public RenderContext
    {
    public:
        RenderContext2D(RenderModule* renderer) : RenderContext(renderer) { };
        virtual ~RenderContext2D() = default;

        virtual void drawRectangle(Camera*, vec2 pos, vec2 size, Color color) = 0;
        virtual void drawRectangle(Camera*, vec2 pos, vec2 size, std::array<Color, 4> colors) = 0;

    };
}
