#include "mesh_asset_to_shape.h"

namespace aiko::physics
{

    TriangleMeshShapeDesc makeTriangleMeshShapeDesc( const MeshAsset& asset )
    {

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
