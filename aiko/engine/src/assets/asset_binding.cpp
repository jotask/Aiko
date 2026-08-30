#include "asset_binding.h"

#include "systems/asset_system.h"
#include "assets/types/mesh_asset.h"
#include "assets/types/texture_asset.h"
#include "assets/types/shader_asset.h"

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
        return m_assetSystem->registerAsset(type, source);
    }

    void AssetBindingContext::loadAsset(AssetType type, const AssetId& id)
    {
        switch (type)
        {
            case AssetType::Texture:
                m_assetSystem->loadAsset<TextureAsset>(id);
                break;

            case AssetType::Mesh:
                m_assetSystem->loadAsset<MeshAsset>(id);
                break;

            case AssetType::Model:
                m_assetSystem->loadAsset<ModelAsset>(id);
                break;

            case AssetType::Shader:
                m_assetSystem->loadAsset<ShaderAsset>(id);
                break;

            case AssetType::ComputeShader:
                m_assetSystem->loadAsset<ComputeShaderAsset>(id);
                break;

            case AssetType::Material:
            case AssetType::Unknown:
            default:
                AIKO_ASSERT(false, "Unsupported asset type");
                break;
        }
    }

    bool AssetBindingContext::isLoaded(AssetType type, const AssetId& id) const
    {
        switch (type)
        {
            case AssetType::Texture:
                return m_assetSystem->isLoaded<TextureAsset>(id);

            case AssetType::Mesh:
                return m_assetSystem->isLoaded<MeshAsset>(id);

            case AssetType::Model:
                return m_assetSystem->isLoaded<ModelAsset>(id);

            case AssetType::Shader:
                return m_assetSystem->isLoaded<ShaderAsset>(id);

            case AssetType::ComputeShader:
                return m_assetSystem->isLoaded<ComputeShaderAsset>(id);

            case AssetType::Material:
            case AssetType::Unknown:
            default:
                AIKO_ASSERT(false, "Unsupported asset type");
                return false;
        }
    }
}
