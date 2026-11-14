#pragma once

#include "aiko_physics_types.h"
#include "aiko_ragdoll_types.h"
#include "jolt_layers.h"
#include "jolt_listeners.h"

#include <Jolt/Physics/Ragdoll/Ragdoll.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <unordered_map>

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

        std::unordered_map<RagdollId, JPH::Ref<JPH::Ragdoll>> ragdolls;
        RagdollId nextRagdollId = 1;

    };
}
