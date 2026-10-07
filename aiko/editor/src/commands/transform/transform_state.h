#pragma once

#include <math/math_vector.h>

namespace aiko::editor
{

    struct TransformState
    {
        vec3 position = {};
        vec3 rotation = {};
        vec3 scale = {1.0f, 1.0f, 1.0f};
    };

}