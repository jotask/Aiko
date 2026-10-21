#pragma once

#include "asset_registry.h"
#include "assets/asset_id.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/texture_asset.h"
#include "assets/types/shader_asset.h"
#include "assets/asset_resolver.h"

#include <aiko_types.h>
#include <assets/iasset_provider.h>

#include <filesystem>
#include <optional>

namespace aiko
{
    class AssetSystem;

    class AssetManager : public IAssetProvider, public IAssetRegistry
    {
        friend class AssetSystem;
    public:
        explicit AssetManager(const AssetResolver& resolver);

        [[nodiscard]]
        AssetResolveResult resolveAsset(std::string_view source) const;

        [[nodiscard]]
        AssetId registerModelTexture(const ResolvedAsset& model, std::string_view textureSource);

        virtual const TextureAsset& getTextureAsset(const AssetId& id) override;
        virtual const MeshAsset& getMeshAsset(const AssetId& id) override;
        virtual const ModelAsset& getModelAsset(const AssetId& id) override;
        virtual const ShaderAsset& getShaderAsset(const AssetId& id) override;
        virtual const ComputeShaderAsset& getComputeShaderAsset(const AssetId& id) override;

        bool loadTextureAsset(const AssetId& id);
        bool loadMeshAsset(const AssetId& id);
        bool loadModelAsset(const AssetId& id);
        void loadShaderAsset(const AssetId& id);
        void loadComputeShaderAsset(const AssetId& id);

        TextureAsset& getMutableTextureAsset(const AssetId& id);

        AssetId registerTexture(std::string_view path) override;
        virtual AssetId registerTexture(const TextureAsset& asset) override;
        virtual AssetId registerTexture(std::string_view path, TextureColorSpace colorSpace);
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

        bool isTextureAssetLoaded(const AssetId& id) const override;
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

        [[nodiscard]]
        std::optional<ShaderAsset> resolveShaderAsset(std::string_view vertexBinary, std::string_view fragmentBinary) const;

        [[nodiscard]]
        std::optional<ComputeShaderAsset> resolveComputeShaderAsset(std::string_view computeBinary) const;

    private:

        AssetId registerShaderAsset(const ShaderAsset& asset);

        const AssetResolver& m_resolver;
        AssetRegistry m_registry;

        std::unordered_map<AssetId, TextureAsset> m_textureAssets;
        std::unordered_map<AssetId, MeshAsset> m_meshAssets;
        std::unordered_map<AssetId, ModelAsset> m_modelAssets;
        std::unordered_map<AssetId, ShaderAsset> m_shaderAssets;
        std::unordered_map<AssetId, ComputeShaderAsset> m_computeShaderAssets;

        std::unordered_map<std::string, AssetId> m_shaderExplicitKeys;

        string getAssetSource(const AssetId& id, AssetType expectedType) const;

        void installTextureAsset(const AssetId& id, TextureAsset asset);
        void installMeshAsset(const AssetId& id, MeshAsset asset);
        void installModelAsset(const AssetId& id, ModelAsset asset);
        void installShaderAsset(const AssetId& id, ShaderAsset asset);
        void installComputeShaderAsset(const AssetId& id, ComputeShaderAsset asset);

        AssetId registerFileAsset(AssetType type, std::string_view source, TextureColorSpace colorSpace = TextureColorSpace::Linear);

    };

}
