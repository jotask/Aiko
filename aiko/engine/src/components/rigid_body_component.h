#pragma once

#include <aiko_physics.h>

#include "models/component.h"
#include "components/physics_debug_proxy.h"

namespace aiko
{
    class PhysicsSystem;

    class RigidBodyComponent : public Component
    {
    public:
        RigidBodyComponent();
        ~RigidBodyComponent() override = default;

        void create(const physics::BodyDesc& desc);

        bool isPhysicsInitialized() const { return m_initialized; }
        physics::BodyId getBodyId() const { return m_bodyId; }

        const Transform& getWorldTransform() const;

        PhysicsDebugProxy& debug() { return m_debug; }
        const PhysicsDebugProxy& debug() const { return m_debug; }

        void ensurePhysicsInitialized(physics::AikoPhysics& world);
        void syncFromPhysics(physics::AikoPhysics& world);
        void physicsShutdown(physics::AikoPhysics& world);

    protected:
        void init() override;
        void dispose() override;

    private:
        physics::BodyDesc m_desc{};
        physics::BodyId m_bodyId = physics::InvalidBodyId;
        physics::AikoPhysics* m_world = nullptr;
        bool m_initialized = false;

        PhysicsDebugProxy m_debug{};
    };
}