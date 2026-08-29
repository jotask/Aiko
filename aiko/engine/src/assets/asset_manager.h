#pragma once

#include "asset_registry.h"
#include "assets/asset_id.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/texture_asset.h"
#include "assets/types/shader_asset.h"

#include <aiko_types.h>
#include <assets/iasset_provider.h>

#include <filesystem>

namespace aiko
{

    class AssetManager : public IAssetProvider, public IAssetRegistry
    {
    public:

        virtual const TextureAsset& getTextureAsset(const AssetId& id) override;
        virtual const MeshAsset& getMeshAsset(const AssetId& id) override;
        virtual const ModelAsset& getModelAsset(const AssetId& id) override;
        virtual const ShaderAsset& getShaderAsset(const AssetId& id) override;
        virtual const ComputeShaderAsset& getComputeShaderAsset(const AssetId& id) override;

        TextureAsset& getMutableTextureAsset(const AssetId& id);

        virtual AssetId registerTexture(std::string_view path) override;
        virtual AssetId registerTexture(const TextureAsset& asset) override;
        virtual AssetId registerMesh(std::string_view path) override;
        virtual AssetId registerMesh(const MeshAsset& asset) override;
        virtual AssetId registerModel(std::string_view path) override;
        virtual AssetId registerShader(std::string_view vsPath,std::string_view fsPath) override;
        virtual AssetId registerShader(std::string_view path) override;
        virtual AssetId registerComputeShader(std::string_view path) override;

        bool hasTextureAsset(const AssetId& id) const;
        bool hasMeshAsset(const AssetId& id) const;
        bool hasModelAsset(const AssetId& id) const;
        bool hasShaderAsset(const AssetId& id) const;
        bool hasComputeShaderAsset(const AssetId& id) const;

        bool isTextureAssetLoaded(const AssetId& id) const;
        bool isMeshAssetLoaded(const AssetId& id) const;
        bool isModelAssetLoaded(const AssetId& id) const;
        bool isShaderAssetLoaded(const AssetId& id) const;
        bool isComputeShaderAssetLoaded(const AssetId& id) const;

        bool loadRegistry(const std::filesystem::path& path);
        bool saveRegistry(const std::filesystem::path& path) const;

        void clear();
        void unloadTexture(const AssetId& id);
        void unloadMesh(const AssetId& id);
        void unloadModel(const AssetId& id);
        void unloadShader(const AssetId& id);
        void unloadComputeShader(const AssetId& id);

    private:

        AssetId registerShaderAsset(const ShaderAsset& asset);

        AssetRegistry m_registry;

        std::unordered_map<AssetId, TextureAsset> m_textureAssets;
        std::unordered_map<AssetId, MeshAsset> m_meshAssets;
        std::unordered_map<AssetId, ModelAsset> m_modelAssets;
        std::unordered_map<AssetId, ShaderAsset> m_shaderAssets;
        std::unordered_map<AssetId, ComputeShaderAsset> m_computeShaderAssets;

        std::unordered_map<std::string, AssetId> m_shaderExplicitKeys;

    };

}