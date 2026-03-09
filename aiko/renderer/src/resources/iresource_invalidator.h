#pragma once
#include "assets/asset_id.h"

namespace aiko
{
    class IRenderResourceInvalidator
    {
    public:
        virtual ~IRenderResourceInvalidator() = default;

        virtual void invalidateTexture(const AssetId& id) = 0;
        virtual void invalidateMesh(const AssetId& id) = 0;
        virtual void invalidateModel(const AssetId& id) = 0;
        virtual void invalidateShader(const AssetId& id) = 0;
    };
}
