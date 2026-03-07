#pragma once

#include "assets/types/mesh_asset.h"
#include "metadata/material_instance.h"
#include "models/material.h"

namespace aiko
{

    class AikoRenderer;

    class MaterialResolver
    {
    public:
        static Material resolve(const MaterialAsset& materialAsset, AikoRenderer& renderer);
        static Material resolve(const MaterialAsset& asset, const MaterialInstance& instance, AikoRenderer& renderer);
    };

}