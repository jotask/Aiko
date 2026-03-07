#pragma once

#include "assets/asset_id.h"
#include "assets/types/texture_asset.h"
#include "assets/types/mesh_asset.h"
#include "types/shader_asset.h"

namespace aiko
{

    class IAssetProvider
    {
    public:
        virtual ~IAssetProvider() = default;

        virtual const TextureAsset& getTextureAsset(const AssetId& id) = 0;
        virtual const MeshAsset& getMeshAsset(const AssetId& id) = 0;
        virtual const ModelAsset& getModelAsset(const AssetId& id) = 0;
        virtual const ShaderAsset& getShaderAsset(const AssetId& id) = 0;
    };

}
