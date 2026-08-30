#pragma once

#include <aiko_physics.h>

namespace aiko
{
    class RigidBodyComponent;
    class PlayerControllerComponent;

    struct PhysicsRigidBodyRuntime
    {
        RigidBodyComponent* component = nullptr;
        physics::AikoPhysics* world = nullptr;
    };

    struct PhysicsPlayerControllerRuntime
    {
        PlayerControllerComponent* component = nullptr;
    };
}
