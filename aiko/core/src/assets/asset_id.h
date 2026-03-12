#pragma once

#include "core/uuid.h"

namespace aiko
{
    using AssetId = uuid::Uuid;
    inline const AssetId InvalidAssetId = AssetId("00000000-0000-0000-0000-000000000000");
}
