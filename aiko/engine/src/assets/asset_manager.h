#pragma once

#include "assets/asset_id.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/texture_asset.h"
#include "assets/types/shader_asset.h"

#include <aiko_types.h>
#include <assets/iasset_provider.h>

namespace aiko
{

    class AssetManager : public IAssetProvider
    {
    public:

        virtual const TextureAsset& getTextureAsset(const AssetId& id) override;
        virtual const MeshAsset& getMeshAsset(const AssetId& id) override;
        virtual const ModelAsset& getModelAsset(const AssetId& id) override;
        virtual const ShaderAsset& getShaderAsset(const AssetId& id) override;
        virtual const ComputeShaderAsset& getComputeShaderAsset(const AssetId& id) override;

        TextureAsset& getMutableTextureAsset(const AssetId& id);

        AssetId registerTexture(std::string_view path);
        AssetId registerTexture(const TextureAsset& asset);
        AssetId registerMesh(std::string_view path);
        AssetId registerMesh(const MeshAsset& asset);
        AssetId registerModel(std::string_view path);
        AssetId registerShader(std::string_view path);
        AssetId registerComputeShader(std::string_view path);

        bool hasTextureAsset(const AssetId& id) const;
        bool hasMeshAsset(const AssetId& id) const;
        bool hasModelAsset(const AssetId& id) const;
        bool hasShaderAsset(const AssetId& id) const;
        bool hasComputeShaderAsset(const AssetId& id) const;

        void clear();
        void unloadTexture(const AssetId& id);
        void unloadMesh(const AssetId& id);
        void unloadModel(const AssetId& id);
        void unloadShader(const AssetId& id);
        void unloadComputeShader(const AssetId& id);

    private:

        std::unordered_map<AssetId, string> m_texturePaths;
        std::unordered_map<AssetId, string> m_meshPaths;
        std::unordered_map<AssetId, string> m_modelPaths;
        std::unordered_map<AssetId, string> m_shaderPaths;
        std::unordered_map<AssetId, string> m_computeShaderPaths;

        std::unordered_map<AssetId, TextureAsset> m_textureAssets;
        std::unordered_map<AssetId, MeshAsset> m_meshAssets;
        std::unordered_map<AssetId, ModelAsset> m_modelAssets;
        std::unordered_map<AssetId, ShaderAsset> m_shaderAssets;
        std::unordered_map<AssetId, ComputeShaderAsset> m_computeShaderAssets;

    };

}