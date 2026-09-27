#include "math_bounds.h"

#include "math_transform.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace aiko::math
{

    Bounds calculateBounds(const vector<vec3>& vertices)
    {
        if (vertices.empty())
        {
            return {};
        }

        Bounds bounds;
        bounds.min = vertices.front();
        bounds.max = vertices.front();

        for (const vec3& vertex : vertices)
        {
            bounds.min.x = std::min(bounds.min.x, vertex.x);
            bounds.min.y = std::min(bounds.min.y, vertex.y);
            bounds.min.z = std::min(bounds.min.z, vertex.z);
            bounds.max.x = std::max(bounds.max.x, vertex.x);
            bounds.max.y = std::max(bounds.max.y, vertex.y);
            bounds.max.z = std::max(bounds.max.z, vertex.z);
        }

        return bounds;
    }

    Bounds transformBounds(const Bounds& bounds, const mat4& transform)
    {
        const vec3 corners[8] =
        {
            {bounds.min.x, bounds.min.y, bounds.min.z},
            {bounds.max.x, bounds.min.y, bounds.min.z},
            {bounds.min.x, bounds.max.y, bounds.min.z},
            {bounds.max.x, bounds.max.y, bounds.min.z},

            {bounds.min.x, bounds.min.y, bounds.max.z},
            {bounds.max.x, bounds.min.y, bounds.max.z},
            {bounds.min.x, bounds.max.y, bounds.max.z},
            {bounds.max.x, bounds.max.y, bounds.max.z}
        };

        const vec3 first = transformPoint(transform, corners[0]);

        Bounds result;
        result.min = first;
        result.max = first;

        for (int i = 1; i < 8; ++i)
        {
            const vec3 corner = transformPoint(transform, corners[i]);
            result.min.x = std::min(result.min.x, corner.x);
            result.min.y = std::min(result.min.y, corner.y);
            result.min.z = std::min(result.min.z, corner.z);
            result.max.x = std::max(result.max.x, corner.x);
            result.max.y = std::max(result.max.y, corner.y);
            result.max.z = std::max(result.max.z, corner.z);
        }

        return result;
    }

    Ray unprojectRay(const vec2& viewportPosition, const mat4& view, const mat4& projection)
    {
        const float x = viewportPosition.x * 2.0f - 1.0f;
        const float y = 1.0f - viewportPosition.y * 2.0f;

        const mat4 inverseViewProjection = inverse(projection * view);

        const vec4 nearClip =
        {
            x,
            y,
            -1.0f,
            1.0f
        };

        const vec4 farClip =
        {
            x,
            y,
            1.0f,
            1.0f
        };

        const vec4 nearWorld4 = transform(inverseViewProjection, nearClip);
        const vec4 farWorld4 = transform(inverseViewProjection, farClip);

        const vec3 nearWorld =
        {
            nearWorld4.x / nearWorld4.w,
            nearWorld4.y / nearWorld4.w,
            nearWorld4.z / nearWorld4.w
        };

        const vec3 farWorld =
        {
            farWorld4.x / farWorld4.w,
            farWorld4.y / farWorld4.w,
            farWorld4.z / farWorld4.w
        };

        Ray ray;

        ray.origin = nearWorld;
        ray.direction = normalize(farWorld - nearWorld);

        return ray;
    }

    bool intersectRayBounds(const Ray& ray, const Bounds& bounds, float& distance)
    {
        float tMin = 0.0f;
        float tMax = std::numeric_limits<float>::max();

        const float origin[3] =
        {
            ray.origin.x,
            ray.origin.y,
            ray.origin.z
        };

        const float direction[3] =
        {
            ray.direction.x,
            ray.direction.y,
            ray.direction.z
        };

        const float min[3] =
        {
            bounds.min.x,
            bounds.min.y,
            bounds.min.z
        };

        const float max[3] =
        {
            bounds.max.x,
            bounds.max.y,
            bounds.max.z
        };

        constexpr float epsilon = 1e-6f;

        for (int axis = 0; axis < 3; ++axis)
        {
            if (std::fabs(direction[axis]) < epsilon)
            {
                if (origin[axis] < min[axis] || origin[axis] > max[axis])
                {
                    return false;
                }

                continue;
            }

            const float inverseDirection = 1.0f / direction[axis];

            float t1 = (min[axis] - origin[axis]) * inverseDirection;
            float t2 = (max[axis] - origin[axis]) * inverseDirection;

            if (t1 > t2)
            {
                std::swap(t1, t2);
            }

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            if (tMin > tMax)
            {
                return false;
            }
        }

        distance = tMin;
        return true;
    }

}
