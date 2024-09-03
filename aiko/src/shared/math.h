#pragma once

#include <iostream> //  For Log::error
#include <iomanip> // For std::setw
#include <cstring> // For std::memcpy

#include "core/log.h"
#include "shared/math_vector.h"
#include "shared/math_transform.h"

namespace aiko
{

    constexpr const double PI = 3.14159265358979323846;
    constexpr const double TWO_PI = 2.0 * PI;

    // Multiplication (float * vec3)
    static vec3 operator*(float scalar, const vec3& v)
    {
        return v * scalar;
    }

    inline float sqrt(float v)
    {
        return std::sqrt(v);
    }

    inline float length(const vec3& v)
    {
        return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

}

