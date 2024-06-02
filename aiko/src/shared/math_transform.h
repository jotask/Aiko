#pragma once

#include "shared/math.h"

namespace aiko
{
    namespace math
    {
        float cos(float);
        float sin(float);
        float radians(float);

        vec3 cross(vec3, vec3);
        vec3 normalize(vec3);

        mat4 scale(mat4, vec3);
        mat4 translate(mat4, vec3);
        mat4 rotate(mat4, float, vec3);
        mat4 lookAt(vec3, vec3, vec3);

        mat4 perspective(float, float, float, float, float);
        mat4 ortho(float, float, float, float, float, float);

    }
}

