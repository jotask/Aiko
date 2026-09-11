#pragma once

#include "aiko_types.h"
#include "assets/asset_id.h"

namespace aiko
{

    struct MeshMeta
    {
        AssetId meshId= InvalidAssetId;
        AssetId materialId= InvalidAssetId;
    };

    struct ModelMeta
    {
        vector<MeshMeta> meshes;
    };

}