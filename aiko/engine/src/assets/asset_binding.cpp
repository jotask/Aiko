#include "asset_binding.h"

#include "systems/asset_system.h"

namespace aiko
{
    AssetBindingContext::AssetBindingContext(AssetSystem& assetSystem)
        : m_assetSystem(&assetSystem)
    {
    }

    AssetId AssetBindingContext::load(AssetType type, string_view source)
    {
        return m_assetSystem->load(type, source);
    }
}
