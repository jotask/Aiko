#pragma once

#include "aiko_physics_types.h"
#include "aiko_query_types.h"
#include "aiko_ragdoll_types.h"

#include <cstddef>
#include <span>

namespace aiko
{

    class SystemConnector;
    class PhysicsSystem;

    class PhysicsContext
    {
    public:
        physics::RagdollId createRagdoll(const physics::RagdollDesc& desc);
        void destroyRagdoll(physics::RagdollId id);
        bool setRagdollPose(physics::RagdollId id, std::span<const Transform> worldTransforms) ;
        size_t getRagdollPartCount(physics::RagdollId id) const;
        physics::BodyId getRagdollPartBodyId(physics::RagdollId id, size_t index) const;

        physics::BodyMeshData getBodyMeshData(physics::BodyId id, bool worldSpace = false) const;
        Transform getBodyTransform(physics::BodyId id) const;

    private:
        friend class LayerContext;

        explicit PhysicsContext(SystemConnector& connector);

        PhysicsSystem* m_physicsSystem = nullptr;
    };
}
