#pragma once

#include "math/math.h"

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


        AIKO_API float cos(float);
        AIKO_API float sin(float);
        AIKO_API float radians(float);

        AIKO_API int max(int, int);

        AIKO_API vec3 cross(vec3, vec3);
        AIKO_API vec3 normalize(vec3);

        AIKO_API mat4 scale(mat4, vec3);
        AIKO_API mat4 translate(mat4, vec3);
        AIKO_API mat4 rotate(mat4, float, vec3);
        AIKO_API vec3 rotate(vec3, float, vec3);
        AIKO_API mat4 lookAt(vec3, vec3, vec3);

        AIKO_API mat4 perspective(float, float, float, float, float);
        AIKO_API mat4 ortho(float, float, float, float, float, float);

    }
}

