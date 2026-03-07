#pragma once

#include <vector>
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
        std::vector<uint32_t>   m_indices;
    };

    struct MaterialAsset
    {
        AssetId shaderId;
        AssetId diffuseTextureId;
        Color baseColor = WHITE;
        bool useVertexColor = false;
        bool lit = true;
    };

    struct ModelAsset
    {
        struct SubMesh
        {
            MeshAsset mesh;
            MaterialAsset material;
        };
        std::vector<SubMesh> submeshes;
    };

}