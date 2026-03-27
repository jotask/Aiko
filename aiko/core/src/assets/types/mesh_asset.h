#pragma once

#include <vector>

#include <assets/asset_id.h>
#include "material_asset.h"
#include "math/math_vector.h"
#include "assets/asset_id.h"

namespace aiko
{
    struct MeshAsset
    {
        std::vector<vec3>       m_vertices;
        std::vector<vec2>       m_textCoord;
        std::vector<vec3>       m_normals;
        std::vector<Color>      m_colors;
        std::vector<uint16_t>   m_indices;
    };

    struct ModelAsset
    {
        struct SubMesh
        {
            AssetId meshId = InvalidAssetId;
            MaterialAsset material;
        };
        std::vector<SubMesh> submeshes;
    };

}