#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#include "player_controller_types.h"

namespace aiko::physics
{
    class AikoPhysics;
    struct PhysicsImpl;

    class PlayerControllerImpl
    {
    public:
        AikoPhysics* world = nullptr;
        PhysicsImpl* worldImpl = nullptr;

        JPH::Ref<JPH::CharacterVirtual> character;

        PlayerControllerDesc desc{};
        float yawRadians = 0.0f;
    };
}