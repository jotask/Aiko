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
        virtual const ComputeShaderAsset& getComputeShaderAsset(const AssetId& id) = 0;
    };

    class IAssetRegistry
    {
    public:
        virtual ~IAssetRegistry() = default;

        virtual AssetId registerTexture(std::string_view path) = 0;
        virtual AssetId registerTexture(const TextureAsset& asset) = 0;
        virtual AssetId registerMesh(std::string_view path) = 0;
        virtual AssetId registerMesh(const MeshAsset& asset) = 0;
        virtual AssetId registerModel(std::string_view path) = 0;
        virtual AssetId registerShader(std::string_view vsPath,std::string_view fsPath) = 0;
        virtual AssetId registerShader(std::string_view path) = 0;
        virtual AssetId registerComputeShader(std::string_view path) = 0;
    };

}
