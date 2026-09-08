#pragma once

#include "assets/types/mesh_asset.h"

#include <aiko_types.h>
#include <types/color.h>

namespace aiko
{
    struct ImportedModelSubMesh
    {
        MeshAsset mesh;

        bool useVertexColor = false;
        bool lit = true;

        Color baseColor{};

        string shaderSource;
        string diffuseTextureSource;
    };

    struct ImportedModel
    {
        vector<ImportedModelSubMesh> submeshes;
    };
}
