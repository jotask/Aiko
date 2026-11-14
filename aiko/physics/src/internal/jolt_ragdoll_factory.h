#pragma once

#include "aiko_ragdoll_types.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Ragdoll/Ragdoll.h>

namespace aiko::physics
{
    class JoltRagdollFactory
    {
    public:
        static JPH::Ref<JPH::RagdollSettings> createSettings(const RagdollDesc& desc);
    };
}
