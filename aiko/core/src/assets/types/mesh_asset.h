#pragma once

#include <aiko_types.h>
#include <assets/asset_id.h>
#include <math/math_vector.h>
#include "material_asset.h"

namespace aiko
{
    struct MeshAsset
    {
        vector<vec3>       m_vertices;
        vector<vec2>       m_textCoord;
        vector<vec3>       m_normals;
        vector<Color>      m_colors;
        vector<uint16_t>   m_indices;
    };

    struct ModelAsset
    {
        struct SubMesh
        {
            AssetId meshId = InvalidAssetId;
            MaterialAsset material;
        };
        vector<SubMesh> submeshes;
    };

}