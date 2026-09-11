#include "jolt_listeners.h"

#include "internal/job_aiko_converters.h"

#include <mutex>

namespace aiko::physics
{
    void PhysicsEventQueue::push(const PhysicsEvent& event)
    {
        std::scoped_lock lock(m_mutex);
        m_events.push_back(event);
    }

    vector<PhysicsEvent> PhysicsEventQueue::drain()
    {
        std::scoped_lock lock(m_mutex);
        vector<PhysicsEvent> out;
        out.swap(m_events);
        return out;
    }

    BodyActivationListenerImpl::BodyActivationListenerImpl(PhysicsEventQueue& queue)
        : m_queue(queue)
    {
    }

    void BodyActivationListenerImpl::OnBodyActivated(const JPH::BodyID& inBodyID, JPH::uint64 /*inBodyUserData*/)
    {
        m_queue.push({
            .type = PhysicsEventType::BodyActivated,
            .bodyA = convert::toAikoBodyId(inBodyID),
            .bodyB = InvalidBodyId
        });
    }

    void BodyActivationListenerImpl::OnBodyDeactivated(const JPH::BodyID& inBodyID, JPH::uint64 /*inBodyUserData*/)
    {
        m_queue.push({
            .type = PhysicsEventType::BodyDeactivated,
            .bodyA = convert::toAikoBodyId(inBodyID),
            .bodyB = InvalidBodyId
        });
    }

    ContactListenerImpl::ContactListenerImpl(PhysicsEventQueue& queue)
        : m_queue(queue)
    {
    }

    void ContactListenerImpl::OnContactAdded(
        const JPH::Body& inBody1,
        const JPH::Body& inBody2,
        const JPH::ContactManifold& /*inManifold*/,
        JPH::ContactSettings& /*ioSettings*/
    )
    {
        m_queue.push({
            .type = PhysicsEventType::ContactAdded,
            .bodyA = convert::toAikoBodyId(inBody1.GetID()),
            .bodyB = convert::toAikoBodyId(inBody2.GetID())
        });
    }

    void ContactListenerImpl::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
    {
        m_queue.push({
            .type = PhysicsEventType::ContactRemoved,
            .bodyA = convert::toAikoBodyId(inSubShapePair.GetBody1ID()),
            .bodyB = convert::toAikoBodyId(inSubShapePair.GetBody2ID())
        });
    }

}
