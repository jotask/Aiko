#include "physics_utils.h"

#include "models/mesh_factory.h"

namespace aiko::lab
{
    physics::BodyDesc boxDesc(vec3 position, vec3 halfExtent, physics::MotionType motion)
    {
        physics::BodyDesc desc{};
        desc.motionType = motion;
        desc.layer = motion == physics::MotionType::Static ? physics::ObjectLayer::NonMoving : physics::ObjectLayer::Moving;
        desc.transform.position = position;
        desc.shape.type = physics::ShapeType::Box;
        desc.shape.box.halfExtent = halfExtent;
        return desc;
    }

    MeshAsset visualMesh(const physics::ShapeDesc& shape)
    {
        MeshAsset result{};
        switch (shape.type)
        {
            case physics::ShapeType::Box:
                result = mesh::factory::generateCube();
                for (vec3& v : result.m_vertices)
                {
                    v.x *= 2.0f * shape.box.halfExtent.x;
                    v.y *= 2.0f * shape.box.halfExtent.y;
                    v.z *= 2.0f * shape.box.halfExtent.z;
                }
                break;
            case physics::ShapeType::Sphere:
                result = mesh::factory::generateMeshSphere(25, 32);
                for (vec3& v : result.m_vertices)
                {
                    v *= 2.0f * shape.sphere.radius;
                }
                break;
            case physics::ShapeType::Capsule:
                result = mesh::factory::generateMeshSphere(25, 32);
                for (vec3& v : result.m_vertices)
                {
                    v *= 2.0f * shape.capsule.radius;
                    v.y += v.y > 0.0f ? shape.capsule.halfHeight : -shape.capsule.halfHeight;
                }
                break;
            case physics::ShapeType::TriangleMesh:
                result.m_vertices = shape.triangleMesh.vertices;
                for (u32 index : shape.triangleMesh.indices)
                {
                    AIKO_ASSERT(index < result.m_vertices.size() && index <= std::numeric_limits<uint16_t>::max(), "Physics Lab visual mesh index is out of range");
                    result.m_indices.push_back(static_cast<uint16_t>(index));
                }
                result.m_textCoord.assign(result.m_vertices.size(), vec2{0.0f});
                result.m_colors.assign(result.m_vertices.size(), WHITE);
                break;
        }
        mesh::factory::recalculateNormals(result);
        return result;
    }

    void configureMaterial(Material& material, const AssetId& shader, Color color)
    {
        material.m_shaderId = shader;
        material.m_baseColor = color;
        material.m_useVertexColor = false;
        material.m_lit = true;
        material.m_renderState.blend = color.a < 1.0f;
        material.m_renderState.depthWrite = !material.m_renderState.blend;
    }

}
