#pragma once

#include <aiko_types.h>

#include <limits>

namespace aiko::physics
{

    using BodyId = u32;
    using CharacterId = u32;
    constexpr BodyId InvalidBodyId = std::numeric_limits<BodyId>::max();
    constexpr CharacterId InvalidCharacterId = std::numeric_limits<CharacterId>::max();

    constexpr uint cTempAllocatorSize = 10 * 1024 * 1024;
    constexpr float kPhysicsDeltaTime = 1.0f / 60.0f;

    enum class MotionType : u8
    {
        Static,
        Kinematic,
        Dynamic
    };

    enum class ObjectLayer : u8
    {
        NonMoving = 0,
        Moving    = 1
    };

    struct WorldDesc
    {
        uint maxBodies = 1024;
        uint numBodyMutexes = 0;
        uint maxBodyPairs = 1024;
        uint maxContactConstraints = 1024;
    };

    struct StepDesc
    {
        float deltaTime = kPhysicsDeltaTime;
        int collisionSteps = 1;
    };

}