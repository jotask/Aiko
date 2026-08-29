#pragma once

#include "systems/base_system.h"

#include "assets/icomponent_assetaccess.h"
#include "resources/iresource_invalidator.h"
#include "assets/asset_type.h"
#include "assets/types/shader_asset.h"

#include <type_traits>

namespace aiko
{

    class RenderModule;
    class AssetsManagerModule;

    class AssetSystem : public BaseSystem, public IComponentAssetAccess, public IRenderResourceInvalidator
    {
    public:
        AssetSystem();
        virtual ~AssetSystem() = default;

        template<typename T>
        AssetId load(string_view source)
        {
            return load(assetType<T>(), source);
        }

        AssetId create(const TextureAsset& asset);
        AssetId create(const MeshAsset& asset);

        // IComponentAssetAccess

        virtual AssetId registerMesh(std::string_view path) override;
        virtual AssetId registerMesh(const MeshAsset& asset) override;
        virtual AssetId registerModel(std::string_view path) override;
        virtual AssetId registerTexture(std::string_view path) override;
        virtual AssetId registerTexture(const TextureAsset& asset) override;
        virtual AssetId registerComputeShader(std::string_view path) override;
        virtual AssetId registerShader(std::string_view path) override;
        virtual AssetId registerShader(std::string_view vs, std::string_view fs) override;

        virtual TextureAsset& getMutableTextureAsset(const AssetId& id) override;

        // IRenderResourceInvalidator

        virtual void invalidateTexture(const AssetId& id) override;
        virtual void invalidateMesh(const AssetId& id) override;
        virtual void invalidateModel(const AssetId& id) override;
        virtual void invalidateShader(const AssetId& id) override;

    protected:

        virtual void connect(ModuleConnector*, SystemConnector*) override;

    private:

        template<typename>
        inline static constexpr bool ALWAYS_FALSE = false;

        template<typename T>
        static consteval AssetType assetType()
        {
            if constexpr (std::is_same_v<T, TextureAsset>)
            {
                return AssetType::Texture;
            }
            else if constexpr (std::is_same_v<T, MeshAsset>)
            {
                return AssetType::Mesh;
            }
            else if constexpr (std::is_same_v<T, ModelAsset>)
            {
                return AssetType::Model;
            }
            else if constexpr (std::is_same_v<T, ShaderAsset>)
            {
                return AssetType::Shader;
            }
            else if constexpr (std::is_same_v<T, ComputeShaderAsset>)
            {
                return AssetType::ComputeShader;
            }
            else
            {
                static_assert(ALWAYS_FALSE<T>, "Unsupported asset type");
            }
        }

        AssetId load(AssetType type, string_view source);

        RenderModule* m_renderModule;
        AssetsManagerModule* m_assetModule;

    };

}
