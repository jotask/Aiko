#pragma once

#include <aiko_types.h>
#include <math/math_vector.h>

namespace aiko::physics
{

    enum class ShapeType : u8
    {
        Sphere,
        Box,
        Capsule,
        TriangleMesh
    };

    struct SphereShapeDesc
    {
        float radius = 0.5f;
    };

    struct BoxShapeDesc
    {
        vec3 halfExtent = vec3(0.5f);
    };

    struct CapsuleShapeDesc
    {
        float halfHeight = 0.5f;
        float radius = 0.5f;
    };

    struct TriangleMeshShapeDesc
    {
        vector<vec3> vertices;
        vector<u32> indices;
    };

    struct ShapeDesc
    {
        ShapeType type = ShapeType::Sphere;
        SphereShapeDesc sphere;
        BoxShapeDesc box;
        CapsuleShapeDesc capsule;
        TriangleMeshShapeDesc triangleMesh;
    };

}