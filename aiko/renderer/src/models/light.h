#pragma once

#include <aiko_types.h>
#include <math/math_vector.h>

#include "types/color.h"

namespace aiko
{

    enum class LightType : uint8_t { Directional, Point, Spot};

    struct AmbientLight
    {
        float intensity;
        Color color;
    };

    struct LightData
    {
        LightType type;
        vec3 position;
        vec3 direction;
        Color color;
        float intensity;

        // point
        float range;

        // spot
        float innerCos;
        float outerCos;

    };

}
