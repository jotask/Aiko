#pragma once
#include "assets/asset_id.h"

namespace aiko
{
    class IRenderResourceInvalidator
    {
    public:
        virtual ~IRenderResourceInvalidator() = default;
        virtual void invalidateTexture(const AssetId& id) = 0;
    };
}
