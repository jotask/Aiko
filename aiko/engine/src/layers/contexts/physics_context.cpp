#include "physics_context.h"

#include "systems/system_connector.h"
#include "systems/physics_system.h"

namespace aiko
{

    PhysicsContext::PhysicsContext(SystemConnector& connector)
    {
        m_physicsSystem = connector.find<PhysicsSystem>();
        AIKO_ASSERT(m_physicsSystem != nullptr, "Required system PhysicsSystem not found");
    }

    physics::RagdollId PhysicsContext::createRagdoll(const physics::RagdollDesc& desc)
    {
        return m_physicsSystem->getPhysics().createRagdoll(desc);
    }

    void PhysicsContext::destroyRagdoll(physics::RagdollId id)
    {
        m_physicsSystem->getPhysics().destroyRagdoll(id);
    }

    bool PhysicsContext::setRagdollPose(physics::RagdollId id, std::span<const Transform> worldTransforms)
    {
        return m_physicsSystem->getPhysics().setRagdollPose(id, worldTransforms);
    }

    size_t PhysicsContext::getRagdollPartCount(physics::RagdollId id) const
    {
        return m_physicsSystem->getPhysics().getRagdollPartCount(id);
    }

    physics::BodyId PhysicsContext::getRagdollPartBodyId(physics::RagdollId id, size_t index) const
    {
        return m_physicsSystem->getPhysics().getRagdollPartBodyId(id, index);
    }

    physics::BodyMeshData PhysicsContext::getBodyMeshData(physics::BodyId id, bool worldSpace) const
    {
        return m_physicsSystem->getPhysics().getBodyMeshData(id, worldSpace);
    }

    Transform PhysicsContext::getBodyTransform(physics::BodyId id) const
    {
        return m_physicsSystem->getPhysics().getTransform(id);
    }

}
