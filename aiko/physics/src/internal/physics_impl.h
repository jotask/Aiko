#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "aiko_physics_types.h"
#include "jolt_layers.h"
#include "jolt_listeners.h"

namespace aiko::physics
{
    struct PhysicsImpl
    {
        BPLayerInterfaceImpl broadPhaseLayerInterface;
        ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;
        ObjectLayerPairFilterImpl objectLayerPairFilter;

        PhysicsEventQueue eventQueue;
        BodyActivationListenerImpl activationListener{eventQueue};
        ContactListenerImpl contactListener{eventQueue};

        AikoUPtr<JPH::TempAllocatorImpl> tempAllocator;
        AikoUPtr<JPH::JobSystemThreadPool> jobSystem;
        AikoUPtr<JPH::PhysicsSystem> physicsSystem;
    };
}
