#include "asset_system.h"

#include "modules/module_connector.h"
#include "modules/assets_manager_module.h"
#include "modules/render_module.h"

namespace aiko
{
    AssetSystem::AssetSystem()
        : m_assetModule(nullptr)
    {

    }

    void AssetSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule);
        BIND_MODULE_REQUIRED(AssetsManagerModule, moduleConnector, m_assetModule);
    }

    AssetId AssetSystem::load(AssetType type, string_view source)
    {
        AssetManager* manager = m_assetModule->getManager();
        switch (type)
        {
            case AssetType::Texture:        return manager->registerTexture(source);
            case AssetType::Mesh:           return manager->registerMesh(source);
            case AssetType::Model:          return manager->registerModel(source);
            case AssetType::Shader:         return manager->registerShader(source);
            case AssetType::ComputeShader:  return manager->registerComputeShader(source);

            case AssetType::Material:
            case AssetType::Unknown:
            default:
                AIKO_ASSERT(false, "Unsupported asset type");
                return InvalidAssetId;
        }
    }

    AssetId AssetSystem::create(const TextureAsset& asset)
    {
        return m_assetModule->getManager()->registerTexture(asset);
    }

    AssetId AssetSystem::create(const MeshAsset& asset)
    {
        return m_assetModule->getManager()->registerMesh(asset);
    }

    TextureAsset& AssetSystem::getMutableTextureAsset(const AssetId& id)
    {
        return m_assetModule->getManager()->getMutableTextureAsset(id);
    }

    void AssetSystem::invalidateTexture(const AssetId& id)
    {
        m_renderModule->getRenderer().resources().updateTexture(id);
    }

    void AssetSystem::invalidateMesh(const AssetId& id)
    {
        m_renderModule->getRenderer().resources().unloadMesh(id);
    }

    void AssetSystem::invalidateModel(const AssetId& id)
    {
        m_renderModule->getRenderer().resources().unloadModel(id);
    }

    void AssetSystem::invalidateShader(const AssetId& id)
    {
        m_renderModule->getRenderer().resources().unloadShader(id);
    }
}
