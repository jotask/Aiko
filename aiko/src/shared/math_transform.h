#pragma once

#include "shared/math.h"

namespace aiko
{
    namespace math
    {

        template<typename T>
        T clamp(T value, T min, T max)
        {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        template<typename T>
        T clamp01(T value)
        {
            return clamp(value, 0, 1);
        }

        float cos(float);
        float sin(float);
        float radians(float);

        vec3 cross(vec3, vec3);
        vec3 normalize(vec3);

        mat4 scale(mat4, vec3);
        mat4 translate(mat4, vec3);
        mat4 rotate(mat4, float, vec3);
        vec3 rotate(vec3, float, vec3);
        mat4 lookAt(vec3, vec3, vec3);

        mat4 perspective(float, float, float, float, float);
        mat4 ortho(float, float, float, float, float, float);

    }
}

