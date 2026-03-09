#pragma once

#include "assets/asset_id.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/texture_asset.h"

namespace aiko
{

    class IComponentAssetAccess
    {
    public:
        virtual ~IComponentAssetAccess() = default;

        virtual AssetId registerMesh(std::string_view path) = 0;
        virtual AssetId registerMesh(const MeshAsset& asset) = 0;
        virtual AssetId registerModel(std::string_view path) = 0;
        virtual AssetId registerTexture(std::string_view path) = 0;
        virtual AssetId registerTexture(const TextureAsset& asset) = 0;
        virtual AssetId registerComputeShader(std::string_view path) = 0;

        virtual TextureAsset& getMutableTextureAsset(const AssetId& id) = 0;
    };

}
