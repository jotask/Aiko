#pragma once

#include "assets/asset_id.h"

namespace aiko
{

    class Texture;

    // Scene/reference material override data.
    // References assets and override values, but does not own runtime GPU resources.
    // Resolved together with a MaterialAsset into a runtime Material.
    struct MaterialInstance
    {
        AssetId shaderId = InvalidAssetId;
        const Texture* runtimeDiffuseTexture = nullptr;
    };

}