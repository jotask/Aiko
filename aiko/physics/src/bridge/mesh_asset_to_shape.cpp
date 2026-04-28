#include "mesh_asset_to_shape.h"

namespace aiko::physics
{

    TriangleMeshShapeDesc makeTriangleMeshShapeDesc( const MeshAsset& asset )
    {

        AIKO_ASSERT(asset.m_vertices.empty() == false, "Mesh has no vertices");
        AIKO_ASSERT(asset.m_indices.empty() == false, "Mesh has no indices");
        AIKO_ASSERT((asset.m_indices.size() % 3) == 0, "Mesh indices are not a triangle list");

        for (uint32_t i = 0; i < asset.m_indices.size(); ++i)
        {
            AIKO_ASSERT(asset.m_indices[i] < asset.m_vertices.size(), "Mesh index out of range");
        }

        for (const vec3& v : asset.m_vertices)
        {
            AIKO_ASSERT(std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z), "Mesh vertex is NaN/Inf");
        }

        TriangleMeshShapeDesc out{};

        if (asset.m_vertices.empty())
        {
            return out;
        }

        if (asset.m_indices.empty())
        {
            return out;
        }

        // Jolt mesh triangles are indexed triangles, so this must be tri-list data.
        if ((asset.m_indices.size() % 3) != 0)
        {
            return out;
        }

        out.vertices = asset.m_vertices;
        out.indices.reserve(asset.m_indices.size());

        for (const u16 index : asset.m_indices)
        {
            out.indices.push_back(static_cast<u32>(index));
        }

        return out;
    }

}
