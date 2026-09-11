#pragma once

#include <aiko_types.h>
#include <math/math_vector.h>

namespace aiko::physics
{
    struct PlayerControllerDesc
    {
        vec3 position = vec3(0.0f);

        float radius = 0.4f;
        float halfHeight = 0.9f;

        float walkSpeed = 6.0f;
        float jumpSpeed = 8.0f;

        float mass = 70.0f;
        float maxStrength = 100.0f;

        float maxSlopeAngleDegrees = 50.0f;
        float characterPadding = 0.02f;
        float predictiveContactDistance = 0.1f;

        float yawRadians = 0.0f;
        uint64_t userData = 0;
    };

    struct PlayerInput
    {
        vec2 move = vec2(0.0f);
        bool jumpPressed = false;
    };
}