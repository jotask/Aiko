#pragma once

#include "math_vector.h"

#include <aiko_types.h>

namespace aiko
{

    struct Bounds
    {
        vec3 min = {};
        vec3 max = {};
    };

    struct Ray
    {
        vec3 origin = {};
        vec3 direction = {};
    };

}

namespace aiko::math
{

    Bounds calculateBounds(const vector<vec3>& vertices);
    Bounds transformBounds(const Bounds& bounds, const mat4& transform);
    Ray unprojectRay(const vec2& viewportPosition, const mat4& view, const mat4& projection);
    bool intersectRayBounds(const Ray& ray, const Bounds& bounds, float& distance);

}
