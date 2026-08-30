#include "asset_binding.h"

#include "systems/asset_system.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/texture_asset.h"

namespace aiko
{
    AssetId AssetBindingContext::create(const MeshAsset& asset)
    {
        return m_assetSystem->create(asset);
    }

    AssetId AssetBindingContext::create(const TextureAsset& asset)
    {
        return m_assetSystem->create(asset);
    }

    TextureAsset& AssetBindingContext::getMutableTexture(const AssetId& id)
    {
        return m_assetSystem->getMutableTextureAsset(id);
    }

    void AssetBindingContext::invalidateTexture(const AssetId& id)
    {
        m_assetSystem->invalidateTexture(id);
    }

    AssetBindingContext::AssetBindingContext(AssetSystem& assetSystem)
        : m_assetSystem(&assetSystem)
    {
    }

    AssetId AssetBindingContext::load(AssetType type, string_view source)
    {
        return m_assetSystem->load(type, source);
    }
}
