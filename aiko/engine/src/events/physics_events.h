#pragma once

#include <aiko_physics_types.h>
#include <core/uuid.h>
#include <events/event.hpp>

namespace aiko
{
    struct PhysicsContactParticipant
    {
        uuid::Uuid objectId;
        uuid::Uuid componentId;
        physics::BodyId bodyId = physics::InvalidBodyId;
    };

    class PhysicsContactStartedEvent final : public Event
    {
    public:
        PhysicsContactStartedEvent(
            const PhysicsContactParticipant& a,
            const PhysicsContactParticipant& b)
            : a(a), b(b)
        {
        }

        PhysicsContactParticipant a;
        PhysicsContactParticipant b;
    };

    class PhysicsContactEndedEvent final : public Event
    {
    public:
        PhysicsContactEndedEvent(
            const PhysicsContactParticipant& a,
            const PhysicsContactParticipant& b)
            : a(a), b(b)
        {
        }

        PhysicsContactParticipant a;
        PhysicsContactParticipant b;
    };
}