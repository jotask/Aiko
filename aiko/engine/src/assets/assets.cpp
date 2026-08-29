#include "assets.h"

#include "asset_manager.h"

namespace aiko
{

    Assets::Assets(AssetManager& manager)
        : m_manager(manager)
    {
    }

    AssetId Assets::load(AssetType type, string_view source)
    {
        switch (type)
        {
            case AssetType::Texture:
                return m_manager.registerTexture(source);

            case AssetType::Mesh:
                return m_manager.registerMesh(source);

            case AssetType::Model:
                return m_manager.registerModel(source);

            case AssetType::Shader:
                return m_manager.registerShader(source);

            case AssetType::ComputeShader:
                return m_manager.registerComputeShader(source);

            case AssetType::Material:
            case AssetType::Unknown:
            default:
                AIKO_ASSERT(false, "Unsupported asset type");
                return InvalidAssetId;
        }
    }

    AssetId Assets::create(const TextureAsset& asset)
    {
        return m_manager.registerTexture(asset);
    }

    AssetId Assets::create(const MeshAsset& asset)
    {
        return m_manager.registerMesh(asset);
    }

}
