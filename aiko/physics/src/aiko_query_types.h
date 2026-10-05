#pragma once

#include <aiko_types.h>
#include <math/math_vector.h>

#include <aiko_physics_types.h>

namespace aiko::physics
{

    struct BodyMeshData
    {
        vector<vec3> vertices;
        vector<u32> indices;
    };

    struct RayCastDesc
    {
        vec3 from = vec3(0.0f);
        vec3 to = vec3(0.0f);
    };

    struct RayCastHit
    {
        BodyId body = InvalidBodyId;
        float fraction = 1.0f;
        vec3 position = vec3(0.0f);
        vec3 normal = vec3(0.0f);
    };

    enum class PhysicsEventType : u8
    {
        BodyActivated,
        BodyDeactivated,
        ContactAdded,
        ContactRemoved
    };

    struct PhysicsEvent
    {
        PhysicsEventType type{};
        BodyId bodyA = InvalidBodyId;
        BodyId bodyB = InvalidBodyId;
    };

    struct SphereOverlapDesc
    {
        vec3 center = vec3(0.0f);
        float radius = 0.5f;
    };

    struct BoxOverlapDesc
    {
        vec3 center = vec3(0.0f);
        vec3 halfExtent = vec3(0.5f);
        vec3 rotation = vec3(0.0f);
    };

}
