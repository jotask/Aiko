#pragma once

#include "systems/base_system.h"

#include "assets/asset_traits.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/shader_asset.h"
#include "assets/types/texture_asset.h"

namespace aiko
{

    class RenderModule;
    class AssetsManagerModule;

    class AssetSystem : public BaseSystem
    {
    public:
        AssetSystem();
        virtual ~AssetSystem() = default;

        template<typename T>
        AssetId load(string_view source)
        {
            return load(assetTypeOf<T>(), source);
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

        AssetId load(AssetType type, string_view source);

        RenderModule* m_renderModule;
        AssetsManagerModule* m_assetModule;

    };

}
