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

    AssetId AssetSystem::registerMesh(std::string_view path)
    {
        return m_assetModule->getManager()->registerMesh(path);
    }

    AssetId AssetSystem::registerMesh(const MeshAsset& asset)
    {
        return m_assetModule->getManager()->registerMesh(asset);
    }

    AssetId AssetSystem::registerModel(std::string_view path)
    {
        return m_assetModule->getManager()->registerModel(path);
    }

    AssetId AssetSystem::registerTexture(std::string_view path)
    {
        return m_assetModule->getManager()->registerTexture(path);
    }

    AssetId AssetSystem::registerTexture(const TextureAsset& asset)
    {
        return m_assetModule->getManager()->registerTexture(asset);
    }

    TextureAsset& AssetSystem::getMutableTextureAsset(const AssetId& id)
    {
        return m_assetModule->getManager()->getMutableTextureAsset(id);
    }

    void AssetSystem::invalidateTexture(const AssetId& id)
    {
        m_renderModule->getRenderer().resources().updateTexture(id);
    }
}
