#include "rigid_body_component.h"

#include "models/game_object.h"

#include "internal/entity_registry_access.h"
#include "internal/physics_runtime_components.h"
#include "scene/entity_registry.h"

namespace aiko
{
    RigidBodyComponent::RigidBodyComponent()
        : Component("RigidBody")
    {
    }

    void RigidBodyComponent::init()
    {
    }

    void RigidBodyComponent::create(const physics::BodyDesc& desc)
    {
        m_desc = desc;
        m_initialized = false;
        m_bodyId = physics::InvalidBodyId;

        m_debug.enabled = desc.debugDraw;
        m_debug.built = false;

        gameobject->transform().position = desc.transform.position;

        entt::registry& registry = EntityRegistryAccess::get(entityRegistry());
        const entt::entity enttEntity = EntityRegistryAccess::toEntity(entity());

        if (!registry.all_of<PhysicsRigidBodyRuntime>(enttEntity))
        {
            registry.emplace<PhysicsRigidBodyRuntime>(
                enttEntity,
                this,
                nullptr
            );
        }
    }

    void RigidBodyComponent::dispose()
    {

        entt::registry& registry = EntityRegistryAccess::get(entityRegistry());
        const entt::entity enttEntity = EntityRegistryAccess::toEntity(entity());

        if (!registry.all_of<PhysicsRigidBodyRuntime>(enttEntity))
        {
            return;
        }

        PhysicsRigidBodyRuntime& runtime = registry.get<PhysicsRigidBodyRuntime>(enttEntity);
        if (runtime.world != nullptr)
        {
            physicsShutdown(*runtime.world);
        }

        registry.remove<PhysicsRigidBodyRuntime>(enttEntity);
    }

    const Transform& RigidBodyComponent::getWorldTransform() const
    {
        return gameobject->transform();
    }

    void RigidBodyComponent::ensurePhysicsInitialized(physics::AikoPhysics& world)
    {
        if (m_initialized == true)
        {
            return;
        }

        m_desc.transform = gameobject->transform();
        m_bodyId = world.createBody(m_desc);

        AIKO_ASSERT(m_bodyId != physics::InvalidBodyId, "RigidBody creation failed");
        m_initialized = true;
    }

    void RigidBodyComponent::syncFromPhysics(physics::AikoPhysics& world)
    {
        if (m_initialized == false)
        {
            return;
        }

        const Transform physicsTransform = world.getTransform(m_bodyId);

        gameobject->transform().position = physicsTransform.position;
        gameobject->transform().rotation = physicsTransform.rotation;
        gameobject->transform().scale = physicsTransform.scale;
    }

    void RigidBodyComponent::physicsShutdown(physics::AikoPhysics& world)
    {
        if (m_initialized == false)
        {
            return;
        }

        world.destroyBody(m_bodyId);
        m_bodyId = physics::InvalidBodyId;
        m_initialized = false;
        m_debug.built = false;
    }
}