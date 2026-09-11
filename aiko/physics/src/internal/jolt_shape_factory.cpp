#include "jolt_shape_factory.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include "Jolt/Physics/Collision/Shape/MeshShape.h"

namespace aiko::physics
{
    JPH::RefConst<JPH::Shape> createJoltShape(const ShapeDesc& desc)
    {
        switch (desc.type)
        {
        case ShapeType::Sphere:
            {
                return new JPH::SphereShape(desc.sphere.radius);
            }

        case ShapeType::Box:
            {
                const vec3& he = desc.box.halfExtent;
                return new JPH::BoxShape(JPH::Vec3(he.x, he.y, he.z));
            }

        case ShapeType::Capsule:
            {
                return new JPH::CapsuleShape(
                    desc.capsule.halfHeight,
                    desc.capsule.radius
                );
            }

        case ShapeType::TriangleMesh:
            {
                const auto& srcVertices = desc.triangleMesh.vertices;
                const auto& srcIndices = desc.triangleMesh.indices;

                if (srcVertices.empty() == true || srcIndices.empty() == true)
                {
                    return nullptr;
                }

                if ((srcIndices.size() % 3) != 0)
                {
                    return nullptr;
                }

                JPH::VertexList vertices;
                vertices.reserve(srcVertices.size());

                for (const vec3& v : srcVertices)
                {
                    vertices.emplace_back(v.x, v.y, v.z);
                }

                JPH::IndexedTriangleList triangles;
                triangles.reserve(srcIndices.size() / 3);

                for (size_t i = 0; i < srcIndices.size(); i += 3)
                {
                    const u32 i0 = srcIndices[i + 0];
                    const u32 i1 = srcIndices[i + 1];
                    const u32 i2 = srcIndices[i + 2];

                    if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size())
                    {
                        return nullptr;
                    }

                    triangles.emplace_back(i0, i1, i2, 0);
                }

                JPH::MeshShapeSettings settings(std::move(vertices), std::move(triangles));
                settings.mBuildQuality = JPH::MeshShapeSettings::EBuildQuality::FavorRuntimePerformance;

                auto shapeResult = settings.Create();
                if (shapeResult.HasError() == true)
                {
                    const JPH::String error = shapeResult.GetError();
                    AIKO_LOG_INFO("Error on creating shape: %s", error.c_str());
                    return nullptr;
                }

                return shapeResult.Get();
            }
        default:
            {
                return nullptr;
            }
        }
    }
}
