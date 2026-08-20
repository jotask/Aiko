#pragma once

#include <math/math_vector.h>

#include "types/color.h"
#include "models/light.h"

namespace aiko::renderer
{

    struct PassDescription
    {
        u32 width;
        u32 height;
        bool clearColor = true;
        bool clearDepth = true;
        Color clear = BLACK;
    };

    struct FrameData
    {
        mat4 view = mat4(1.0f);
        mat4 projection = mat4(1.0f);
        vec3 cameraPosition = vec3(0.0f);

        AmbientLight ambient;

        vector<LightData> lights;

    };

}