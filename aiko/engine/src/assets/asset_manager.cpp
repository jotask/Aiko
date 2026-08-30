#include "asset_manager.h"

#include "asset_importer.h"
#include "asset_registry_serializer.h"

namespace aiko
{

    const TextureAsset& AssetManager::getTextureAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get texture from invalid UUID");

        auto it = m_textureAssets.find(id);
        if (it == m_textureAssets.end())
        {
            loadTextureAsset(id);
            it = m_textureAssets.find(id);
        }

        AIKO_ASSERT(it != m_textureAssets.end(), "Texture asset failed to load");
        return it->second;

    }

    const MeshAsset& AssetManager::getMeshAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get mesh from invalid UUID");

        auto it = m_meshAssets.find(id);
        if (it == m_meshAssets.end())
        {
            loadMeshAsset(id);
            it = m_meshAssets.find(id);
        }

        AIKO_ASSERT(it != m_meshAssets.end(), "Mesh asset failed to load");
        return it->second;
    }

    const ModelAsset& AssetManager::getModelAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get model from invalid UUID");

        auto it = m_modelAssets.find(id);
        if (it == m_modelAssets.end())
        {
            loadModelAsset(id);
            it = m_modelAssets.find(id);
        }

        AIKO_ASSERT(it != m_modelAssets.end(), "Model asset failed to load");
        return it->second;
    }

    const ShaderAsset& AssetManager::getShaderAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get shader from invalid UUID");

        auto it = m_shaderAssets.find(id);
        if (it == m_shaderAssets.end())
        {
            loadShaderAsset(id);
            it = m_shaderAssets.find(id);
        }

        AIKO_ASSERT(it != m_shaderAssets.end(), "Shader asset failed to load");
        return it->second;
    }

    const ComputeShaderAsset& AssetManager::getComputeShaderAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get compute shader from invalid UUID");

        auto it = m_computeShaderAssets.find(id);
        if (it == m_computeShaderAssets.end())
        {
            loadComputeShaderAsset(id);
            it = m_computeShaderAssets.find(id);
        }

        AIKO_ASSERT(it != m_computeShaderAssets.end(), "Compute shader asset failed to load");
        return it->second;
    }

    void AssetManager::loadTextureAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to load texture from invalid UUID");

        if (m_textureAssets.find(id) != m_textureAssets.end())
        {
            return;
        }

        const AssetRecord* record = m_registry.find(id);
        AIKO_ASSERT(record != nullptr && record->type == AssetType::Texture, "Texture asset id not registered");

        TextureAsset asset = AssetImporter::loadTexture(record->source, this);
        m_textureAssets.emplace(id, std::move(asset));
    }

    void AssetManager::loadMeshAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to load mesh from invalid UUID");

        if (m_meshAssets.find(id) != m_meshAssets.end())
        {
            return;
        }

        const AssetRecord* record = m_registry.find(id);
        AIKO_ASSERT(record != nullptr && record->type == AssetType::Mesh, "Mesh asset id not registered");

        MeshAsset asset = AssetImporter::loadMesh(record->source, this);
        m_meshAssets.emplace(id, std::move(asset));
    }

    void AssetManager::loadModelAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to load model from invalid UUID");

        if (m_modelAssets.find(id) != m_modelAssets.end())
        {
            return;
        }

        const AssetRecord* record = m_registry.find(id);
        AIKO_ASSERT(record != nullptr && record->type == AssetType::Model, "Model asset id not registered");

        ModelAsset asset = AssetImporter::loadModel(record->source, this);
        m_modelAssets.emplace(id, std::move(asset));
    }

    void AssetManager::loadShaderAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to load shader from invalid UUID");

        if (m_shaderAssets.find(id) != m_shaderAssets.end())
        {
            return;
        }

        const AssetRecord* record = m_registry.find(id);
        AIKO_ASSERT(record != nullptr && record->type == AssetType::Shader, "Shader asset id not registered");

        ShaderAsset asset = AssetImporter::loadShader(record->source, this);
        m_shaderAssets.emplace(id, std::move(asset));
    }

    void AssetManager::loadComputeShaderAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to load compute shader from invalid UUID");

        if (m_computeShaderAssets.find(id) != m_computeShaderAssets.end())
        {
            return;
        }

        const AssetRecord* record = m_registry.find(id);
        AIKO_ASSERT(record != nullptr && record->type == AssetType::ComputeShader, "Compute shader asset id not registered");

        ComputeShaderAsset asset = AssetImporter::loadComputeShader(record->source, this);
        m_computeShaderAssets.emplace(id, std::move(asset));
    }

    TextureAsset& AssetManager::getMutableTextureAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get texture from invalid UUID");

        auto it = m_textureAssets.find(id);
        if (it == m_textureAssets.end())
        {
            loadTextureAsset(id);
            it = m_textureAssets.find(id);
        }

        AIKO_ASSERT(it != m_textureAssets.end(), "Texture asset failed to load");
        return it->second;
    }

    AssetId AssetManager::registerTexture(std::string_view path)
    {
        return m_registry.registerAsset(AssetType::Texture, path);
    }

    AssetId AssetManager::registerTexture(const TextureAsset& asset)
    {
        AssetId id;
        m_textureAssets.emplace(id, asset);
        return id;
    }

    AssetId AssetManager::registerMesh(std::string_view path)
    {
        return m_registry.registerAsset(AssetType::Mesh, path);
    }

    AssetId AssetManager::registerMesh(const MeshAsset& asset)
    {
        AssetId id;
        m_meshAssets.emplace(id, asset);
        return id;
    }

    AssetId AssetManager::registerModel(std::string_view path)
    {
        return m_registry.registerAsset(AssetType::Model, path);
    }

    AssetId AssetManager::registerShader(std::string_view vsPath, std::string_view fsPath)
    {
        ShaderAsset asset{};
        asset.vertexPath = string(vsPath);
        asset.fragmentPath = string(fsPath);
        return registerShaderAsset(asset);
    }

    AssetId AssetManager::registerShader(std::string_view path)
    {
        const string p(path);
        const string explicitKey = p + ".vs|" + p + ".fs";
        auto explicitIt = m_shaderExplicitKeys.find(explicitKey);
        if (explicitIt != m_shaderExplicitKeys.end())
        {
            return explicitIt->second;
        }
        return m_registry.registerAsset(AssetType::Shader, path);
    }

    AssetId AssetManager::registerComputeShader(std::string_view path)
    {
        return m_registry.registerAsset(AssetType::ComputeShader, path);
    }

    bool AssetManager::hasTextureAsset(const AssetId& id) const
    {
        if (m_textureAssets.find(id) != m_textureAssets.end())
        {
            return true;
        }
        const AssetRecord* record = m_registry.find(id);
        return record != nullptr && record->type == AssetType::Texture;
    }

    bool AssetManager::hasMeshAsset(const AssetId& id) const
    {
        if (m_meshAssets.find(id) != m_meshAssets.end())
        {
            return true;
        }
        const AssetRecord* record = m_registry.find(id);
        return record != nullptr && record->type == AssetType::Mesh;
    }

    bool AssetManager::hasModelAsset(const AssetId& id) const
    {
        if (m_modelAssets.find(id) != m_modelAssets.end())
        {
            return true;
        }
        const AssetRecord* record = m_registry.find(id);
        return record != nullptr && record->type == AssetType::Model;
    }

    bool AssetManager::hasShaderAsset(const AssetId& id) const
    {
        if (m_shaderAssets.find(id) != m_shaderAssets.end())
        {
            return true;
        }
        const AssetRecord* record = m_registry.find(id);
        return record != nullptr && record->type == AssetType::Shader;
    }

    bool AssetManager::hasComputeShaderAsset(const AssetId& id) const
    {
        if (m_computeShaderAssets.find(id) != m_computeShaderAssets.end())
        {
            return true;
        }
        const AssetRecord* record = m_registry.find(id);
        return record != nullptr && record->type == AssetType::ComputeShader;
    }

    bool AssetManager::isTextureAssetLoaded(const AssetId& id) const
    {
        return m_textureAssets.find(id) != m_textureAssets.end();
    }

    bool AssetManager::isMeshAssetLoaded(const AssetId& id) const
    {
        return m_meshAssets.find(id) != m_meshAssets.end();
    }

    bool AssetManager::isModelAssetLoaded(const AssetId& id) const
    {
        return m_modelAssets.find(id) != m_modelAssets.end();
    }

    bool AssetManager::isShaderAssetLoaded(const AssetId& id) const
    {
        return m_shaderAssets.find(id) != m_shaderAssets.end();
    }

    bool AssetManager::isComputeShaderAssetLoaded(const AssetId& id) const
    {
        return m_computeShaderAssets.find(id) != m_computeShaderAssets.end();
    }

    bool AssetManager::loadRegistry(const std::filesystem::path& path)
    {
        return AssetRegistrySerializer::load(m_registry, path);
    }

    bool AssetManager::saveRegistry(const std::filesystem::path& path) const
    {
        return AssetRegistrySerializer::save(m_registry, path);
    }

    void AssetManager::clear()
    {
        m_textureAssets.clear();
        m_meshAssets.clear();
        m_modelAssets.clear();
        m_shaderAssets.clear();
        m_computeShaderAssets.clear();
        m_registry.clear();
        m_shaderExplicitKeys.clear();
    }

    void AssetManager::unloadTexture(const AssetId& id)
    {
        m_textureAssets.erase(id);
    }

    void AssetManager::unloadMesh(const AssetId& id)
    {
        m_meshAssets.erase(id);
    }

    void AssetManager::unloadModel(const AssetId& id)
    {
        m_modelAssets.erase(id);
    }

    void AssetManager::unloadShader(const AssetId& id)
    {
        auto it = m_shaderAssets.find(id);
        if (it != m_shaderAssets.end())
        {
            const ShaderAsset& asset = it->second;
            if (asset.vertexPath.empty() == false && asset.fragmentPath.empty() == false)
            {
                const string key = asset.vertexPath + "|" + asset.fragmentPath;
                m_shaderExplicitKeys.erase(key);
            }
            m_shaderAssets.erase(it);
            return;
        }
    }

    void AssetManager::unloadComputeShader(const AssetId& id)
    {
        m_computeShaderAssets.erase(id);
    }

    AssetId AssetManager::registerShaderAsset(const ShaderAsset& asset)
    {
        AIKO_ASSERT(asset.vertexPath.empty() == false, "ShaderAsset vertexPath is empty");
        AIKO_ASSERT(asset.fragmentPath.empty() == false, "ShaderAsset fragmentPath is empty");

        const string key = asset.vertexPath + "|" + asset.fragmentPath;

        auto it = m_shaderExplicitKeys.find(key);
        if (it != m_shaderExplicitKeys.end())
        {
            return it->second;
        }

        AssetId id;
        m_shaderAssets.emplace(id, asset);
        m_shaderExplicitKeys.emplace(key, id);
        return id;
    }
}
