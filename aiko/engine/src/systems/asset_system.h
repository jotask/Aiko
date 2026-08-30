#pragma once

#include "systems/base_system.h"

#include "assets/asset_traits.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/shader_asset.h"
#include "assets/types/texture_asset.h"

#include <type_traits>

namespace aiko
{

    class RenderModule;
    class AssetsManagerModule;
    class AssetBindingContext;

    class AssetSystem : public BaseSystem
    {
        friend class AssetBindingContext;
    public:
        AssetSystem();
        virtual ~AssetSystem() = default;

        template<typename T>
        AssetId registerAsset(string_view source)
        {
            return registerAsset(assetTypeOf<T>(), source);
        }

        template<typename T>
        const T& get(const AssetId& id)
        {
            return get(id, std::type_identity<T>{});
        }

        template<typename T>
        bool isLoaded(const AssetId& id) const
        {
            return isLoaded(id, std::type_identity<T>{});
        }

        AssetId create(const TextureAsset& asset);
        AssetId create(const MeshAsset& asset);

        TextureAsset& getMutableTextureAsset(const AssetId& id);

        void invalidateTexture(const AssetId& id);
        void invalidateMesh(const AssetId& id);
        void invalidateModel(const AssetId& id);
        void invalidateShader(const AssetId& id);

    protected:

        virtual void connect(ModuleConnector*, SystemConnector*) override;

    private:

        AssetId registerAsset(AssetType type, string_view source);

        const TextureAsset& get(const AssetId& id, std::type_identity<TextureAsset>);
        const MeshAsset& get(const AssetId& id, std::type_identity<MeshAsset>);
        const ModelAsset& get(const AssetId& id, std::type_identity<ModelAsset>);
        const ShaderAsset& get(const AssetId& id, std::type_identity<ShaderAsset>);
        const ComputeShaderAsset& get(const AssetId& id, std::type_identity<ComputeShaderAsset>);

        bool isLoaded(const AssetId& id, std::type_identity<TextureAsset>) const;
        bool isLoaded(const AssetId& id, std::type_identity<MeshAsset>) const;
        bool isLoaded(const AssetId& id, std::type_identity<ModelAsset>) const;
        bool isLoaded(const AssetId& id, std::type_identity<ShaderAsset>) const;
        bool isLoaded(const AssetId& id, std::type_identity<ComputeShaderAsset>) const;

        RenderModule* m_renderModule;
        AssetsManagerModule* m_assetModule;

    };

}
