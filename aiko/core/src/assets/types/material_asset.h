#pragma once

#include "assets/asset_id.h"

namespace aiko
{
    struct MaterialAsset
    {
        AssetId shaderId = InvalidAssetId;
        AssetId diffuseTextureId = InvalidAssetId;
        Color baseColor = WHITE;
        bool useVertexColor = false;
        bool lit = true;
    };
}