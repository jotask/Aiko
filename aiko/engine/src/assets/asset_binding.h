#pragma once

#include <aiko_types.h>

#include "assets/asset_id.h"
#include "assets/asset_traits.h"

namespace aiko
{
    class AssetBindingSystem;
    class AssetSystem;

    class AssetBindingContext
    {
        friend class AssetBindingSystem;

    public:
        template<typename T>
        AssetId load(string_view source)
        {
            return load(assetTypeOf<T>(), source);
        }

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
        virtual void resolveAssetBinding(AssetBindingContext& context) = 0;
    };
}
