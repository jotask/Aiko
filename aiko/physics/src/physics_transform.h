#pragma once

#include <core/transform.h>

namespace aiko::physics
{
    Transform toPhysicsTransform(const Transform& transform);

    void applyPhysicsTransform(Transform& transform, const Transform& physicsTransform);
}
