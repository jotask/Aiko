#pragma once

#include "types/color.h"
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
        float specularStrength = 0.0f;
        float shininess = 32.0f;
    };
}
