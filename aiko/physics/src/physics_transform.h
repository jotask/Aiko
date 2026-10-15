#pragma once

#include <core/transform.h>

namespace aiko::physics
{
    Transform toPhysicsTransform(const Transform& transform);

    bool applyPhysicsTransform(Transform& transform, const Transform& physicsTransform);
}
