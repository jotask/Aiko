#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/ContactListener.h>

#include "aiko_query_types.h"

namespace aiko::physics
{

    class PhysicsEventQueue
    {
    public:
        void push(const PhysicsEvent & event);
        vector<PhysicsEvent> drain();
    private:
        std::mutex m_mutex;
        vector<PhysicsEvent> m_events;
    };

    class BodyActivationListenerImpl final : public JPH::BodyActivationListener
    {
    public:
        explicit BodyActivationListenerImpl(PhysicsEventQueue& queue);
        virtual void OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override;
        virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 inBodyUserData) override;
    private:
        PhysicsEventQueue& m_queue;
    };

    class ContactListenerImpl final : public JPH::ContactListener
    {
    public:
        explicit ContactListenerImpl(PhysicsEventQueue& queue);
        virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
        virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;
    private:
        PhysicsEventQueue& m_queue;
    };

}
