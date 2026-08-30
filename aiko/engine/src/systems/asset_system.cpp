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

    AssetId AssetSystem::registerAsset(AssetType type, string_view source)
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

    const TextureAsset& AssetSystem::get(const AssetId& id, std::type_identity<TextureAsset>)
    {
        return m_assetModule->getManager()->getTextureAsset(id);
    }

    const MeshAsset& AssetSystem::get(const AssetId& id, std::type_identity<MeshAsset>)
    {
        return m_assetModule->getManager()->getMeshAsset(id);
    }

    const ModelAsset& AssetSystem::get(const AssetId& id, std::type_identity<ModelAsset>)
    {
        return m_assetModule->getManager()->getModelAsset(id);
    }

    const ShaderAsset& AssetSystem::get(const AssetId& id, std::type_identity<ShaderAsset>)
    {
        return m_assetModule->getManager()->getShaderAsset(id);
    }

    const ComputeShaderAsset& AssetSystem::get(const AssetId& id, std::type_identity<ComputeShaderAsset>)
    {
        return m_assetModule->getManager()->getComputeShaderAsset(id);
    }

    bool AssetSystem::isLoaded(const AssetId& id, std::type_identity<TextureAsset>) const
    {
        return m_assetModule->getManager()->isTextureAssetLoaded(id);
    }

    bool AssetSystem::isLoaded(const AssetId& id, std::type_identity<MeshAsset>) const
    {
        return m_assetModule->getManager()->isMeshAssetLoaded(id);
    }

    bool AssetSystem::isLoaded(const AssetId& id, std::type_identity<ModelAsset>) const
    {
        return m_assetModule->getManager()->isModelAssetLoaded(id);
    }

    bool AssetSystem::isLoaded(const AssetId& id, std::type_identity<ShaderAsset>) const
    {
        return m_assetModule->getManager()->isShaderAssetLoaded(id);
    }

    bool AssetSystem::isLoaded(const AssetId& id, std::type_identity<ComputeShaderAsset>) const
    {
        return m_assetModule->getManager()->isComputeShaderAssetLoaded(id);
    }

    void AssetSystem::loadAsset(const AssetId& id, std::type_identity<TextureAsset>)
    {
        m_assetModule->getManager()->loadTextureAsset(id);
    }

    void AssetSystem::loadAsset(const AssetId& id, std::type_identity<MeshAsset>)
    {
        m_assetModule->getManager()->loadMeshAsset(id);
    }

    void AssetSystem::loadAsset(const AssetId& id, std::type_identity<ModelAsset>)
    {
        m_assetModule->getManager()->loadModelAsset(id);
    }

    void AssetSystem::loadAsset(const AssetId& id, std::type_identity<ShaderAsset>)
    {
        m_assetModule->getManager()->loadShaderAsset(id);
    }

    void AssetSystem::loadAsset(const AssetId& id, std::type_identity<ComputeShaderAsset>)
    {
        m_assetModule->getManager()->loadComputeShaderAsset(id);
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
