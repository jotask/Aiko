#pragma once

#include <aiko_types.h>

#include "assets/asset_id.h"
#include "assets/asset_traits.h"

namespace aiko
{
    class AssetBindingSystem;
    class AssetSystem;

    struct TextureAsset;
    struct MeshAsset;

    class AssetBindingContext
    {
        friend class AssetBindingSystem;

    public:
        template<typename T>
        AssetId load(string_view source)
        {
            return load(assetTypeOf<T>(), source);
        }

        template<typename T>
        void loadAsset(const AssetId& id)
        {
            loadAsset(assetTypeOf<T>(), id);
        }

        template<typename T>
        bool isLoaded(const AssetId& id) const
        {
            return isLoaded(assetTypeOf<T>(), id);
        }

        AssetId create(const MeshAsset& asset);
        AssetId create(const TextureAsset& asset);
        TextureAsset& getMutableTexture(const AssetId& id);
        void invalidateTexture(const AssetId& id);

    private:
        explicit AssetBindingContext(AssetSystem& assetSystem);

        AssetId load(AssetType type, string_view source);
        void loadAsset(AssetType type, const AssetId& id);
        bool isLoaded(AssetType type, const AssetId& id) const;

        AssetSystem* m_assetSystem = nullptr;
    };

    class IAssetBinding
    {
        friend class AssetBindingSystem;

    public:
        virtual ~IAssetBinding() = default;

    private:
        virtual void resolveAssetBinding(AssetBindingContext& context) = 0;
    };
}
