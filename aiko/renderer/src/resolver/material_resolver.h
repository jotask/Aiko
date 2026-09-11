#pragma once

#include "assets/iasset_provider.h"
#include "assets/types/mesh_asset.h"
#include "metadata/material_instance.h"
#include "models/material.h"

namespace aiko
{

    class AikoRenderer;

    class MaterialResolver
    {
    public:
        static AikoUPtr<Material> resolve(const MaterialAsset& materialAsset, IAssetProvider& assets, AikoRenderer& renderer);
        static AikoUPtr<Material> resolve(const MaterialAsset& asset, const MaterialInstance& instance, IAssetProvider& assets, AikoRenderer& renderer);
    };

}