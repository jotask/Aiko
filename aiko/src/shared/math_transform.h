#pragma once

#include "shared/math.h"

namespace aiko
{
    namespace math
    {
        mat4 translate(mat4, vec3);
        mat4 rotate(mat4, float, vec3);
        vec3 normalize(vec3);
    }
}

