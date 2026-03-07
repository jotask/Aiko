#pragma once

#include "aiko_types.h"
#include "assets/asset_id.h"

#include <vector>

namespace aiko
{

    struct MeshMeta
    {
        AssetId meshId= InvalidAssetId;
        AssetId materialId= InvalidAssetId;
    };

    struct ModelMeta
    {
        std::vector<MeshMeta> meshes;
    };

}