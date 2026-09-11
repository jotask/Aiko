#pragma once

#include "aiko_types.h"

#include "assets/asset_id.h"
#include "assets/asset_type.h"

namespace aiko
{

    struct AssetRecord
    {
        AssetId id = InvalidAssetId;
        AssetType type = AssetType::Unknown;
        string source;
    };

}
