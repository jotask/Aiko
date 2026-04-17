#pragma once

#include <core/transform.h>

#include "aiko_physics_types.h"
#include "aiko_shape_desc.h"

namespace aiko::physics
{
    struct BodyDesc
    {
        MotionType motionType = MotionType::Static;
        ObjectLayer layer = ObjectLayer::NonMoving;

        Transform transform{};
        ShapeDesc shape{};

        float mass = 1.0f;
        float restitution = 0.0f;
        float friction = 0.2f;

        bool isSensor = false;

        bool activate = true;

    };
}