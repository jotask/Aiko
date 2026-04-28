#pragma once

#include "aiko_physics_types.h"
#include "aiko_body_desc.h"
#include "aiko_query_types.h"

#include <aiko_types.h>
#include <core/transform.h>

#include <optional>

#include "player_controller_types.h"
namespace aiko::physics
{

    struct PhysicsImpl;
    class PlayerController;

    class AikoPhysics
    {
        friend class PlayerController;
    public:

        AikoPhysics();
        ~AikoPhysics();

        AikoPhysics(AikoPhysics&&) noexcept;
        AikoPhysics& operator=(AikoPhysics&&) noexcept;

        AikoPhysics(const AikoPhysics&) = delete;
        AikoPhysics& operator=(const AikoPhysics&) = delete;

        void init(const WorldDesc& desc = {});
        void shutdown();

        void step(const StepDesc& desc = {});

        BodyId createBody(const BodyDesc& desc);
        void destroyBody(BodyId id);

        void setTransform(BodyId id, const Transform& transform);
        Transform getTransform(BodyId id) const;

        void setLinearVelocity(BodyId id, const vec3& velocity);
        vec3 getLinearVelocity(BodyId id) const;
        void addImpulse(BodyId id, const vec3& impulse);
        void setAngularVelocity(BodyId id, const vec3& velocity);
        vec3 getAngularVelocity(BodyId id) const;
        void addForce(BodyId id, const vec3& force);
        void addTorque(BodyId id, const vec3& torque);
        void addAngularImpulse(BodyId id, const vec3& angularImpulse);

        void moveKinematic(BodyId id, const Transform& target, float deltaTime);
        void setMotionType(BodyId id, MotionType motionType, bool activate = true);
        MotionType getMotionType(BodyId id) const;

        void setIsSensor(BodyId id, bool isSensor);
        bool isSensor(BodyId id) const;

        void setGravityFactor(BodyId id, float gravityFactor);
        float getGravityFactor(BodyId id) const;

        BodyMeshData getBodyMeshData(BodyId id, bool worldSpace = false) const;

        bool castRay(const RayCastDesc& desc, RayCastHit& outHit) const;

        vector<PhysicsEvent> drainEvents();

    private:
        AikoUPtr<PhysicsImpl> m_impl;
    };

}
