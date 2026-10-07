#pragma once

#include <events/event.hpp>

namespace aiko
{
    class GameObject;
    class RigidBodyComponent;

    class PhysicsContactStartedEvent final : public Event
    {
    public:
        PhysicsContactStartedEvent(GameObject* objectA, GameObject* objectB, RigidBodyComponent* bodyA, RigidBodyComponent* bodyB)
            : objectA(objectA)
            , objectB(objectB)
            , bodyA(bodyA)
            , bodyB(bodyB)
        {
        }

        GameObject* objectA = nullptr;
        GameObject* objectB = nullptr;

        RigidBodyComponent* bodyA = nullptr;
        RigidBodyComponent* bodyB = nullptr;
    };

    class PhysicsContactEndedEvent final : public Event
    {
    public:
        PhysicsContactEndedEvent(GameObject* objectA, GameObject* objectB, RigidBodyComponent* bodyA, RigidBodyComponent* bodyB)
            : objectA(objectA)
            , objectB(objectB)
            , bodyA(bodyA)
            , bodyB(bodyB)
        {
        }

        GameObject* objectA = nullptr;
        GameObject* objectB = nullptr;

        RigidBodyComponent* bodyA = nullptr;
        RigidBodyComponent* bodyB = nullptr;
    };
}
