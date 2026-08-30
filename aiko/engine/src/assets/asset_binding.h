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

        AssetId create(const MeshAsset& asset);
        AssetId create(const TextureAsset& asset);
        TextureAsset& getMutableTexture(const AssetId& id);
        void invalidateTexture(const AssetId& id);

    private:
        explicit AssetBindingContext(AssetSystem& assetSystem);

        AssetId load(AssetType type, string_view source);

        AssetSystem* m_assetSystem = nullptr;
    };

    class IAssetBinding
    {
        friend class AssetBindingSystem;

    public:
        virtual ~IAssetBinding() = default;

    private:
        virtual bool resolveAssetBinding(AssetBindingContext& context) = 0;
    };
}
