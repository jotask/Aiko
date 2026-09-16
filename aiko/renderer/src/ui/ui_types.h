#pragma once

#include <math/math.h>
#include <types/color.h>

namespace aiko
{

    struct UIRect
    {
        vec2 position{0.0f};
        vec2 size{0.0f};
    };

    struct UIVertex
    {
        vec2 position{0.0f};
        vec2 uv{0.0f};
        Color color = WHITE;
    };

}
