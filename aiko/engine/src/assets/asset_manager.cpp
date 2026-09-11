#include "asset_manager.h"

#include "asset_importer.h"
#include "asset_registry_serializer.h"

namespace aiko
{

    const TextureAsset& AssetManager::getTextureAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get texture from invalid UUID");

        const auto it = m_textureAssets.find(id);

        AIKO_ASSERT(it != m_textureAssets.end(), "Texture asset requested before loading");
        return it->second;

    }

    const MeshAsset& AssetManager::getMeshAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get mesh from invalid UUID");

        const auto it = m_meshAssets.find(id);

        AIKO_ASSERT(it != m_meshAssets.end(), "Mesh asset requested before loading");
        return it->second;
    }

    const ModelAsset& AssetManager::getModelAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get model from invalid UUID");

        const auto it = m_modelAssets.find(id);

        AIKO_ASSERT(it != m_modelAssets.end(), "Model asset requested before loading");
        return it->second;
    }

    const ShaderAsset& AssetManager::getShaderAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get shader from invalid UUID");

        const auto it = m_shaderAssets.find(id);

        AIKO_ASSERT(it != m_shaderAssets.end(), "Shader asset requested before loading");
        return it->second;
    }

    const ComputeShaderAsset& AssetManager::getComputeShaderAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get compute shader from invalid UUID");

        const auto it = m_computeShaderAssets.find(id);

        AIKO_ASSERT(it != m_computeShaderAssets.end(), "Compute shader asset requested before loading");
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

        TextureAsset asset = AssetImporter::loadTexture(record->source);
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

        MeshAsset asset = AssetImporter::loadMesh(record->source);
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

        ImportedModel imported = AssetImporter::loadModel(record->source);

        ModelAsset asset{};

        for (ImportedModelSubMesh& importedSubmesh : imported.submeshes)
        {
            ModelAsset::SubMesh submesh{};

            submesh.meshId = registerMesh(importedSubmesh.mesh);
            submesh.material.useVertexColor = importedSubmesh.useVertexColor;
            submesh.material.lit = importedSubmesh.lit;
            submesh.material.baseColor = importedSubmesh.baseColor;
            submesh.material.shaderId = registerShader(importedSubmesh.shaderSource);

            submesh.material.diffuseTextureId = InvalidAssetId;

            if (!importedSubmesh.diffuseTextureSource.empty())
            {
                submesh.material.diffuseTextureId = registerTexture(importedSubmesh.diffuseTextureSource);
            }

            asset.submeshes.push_back(std::move(submesh));
        }

        for (const ModelAsset::SubMesh& submesh : asset.submeshes)
        {
            AIKO_ASSERT(submesh.meshId != InvalidAssetId, "Model submesh has invalid mesh asset id");

            loadMeshAsset(submesh.meshId);

            AIKO_ASSERT(submesh.material.shaderId != InvalidAssetId, "Model submesh has invalid shader asset id");

            loadShaderAsset(submesh.material.shaderId);

            if (submesh.material.diffuseTextureId != InvalidAssetId)
            {
                loadTextureAsset(submesh.material.diffuseTextureId);
            }
        }

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

        ShaderAsset asset = AssetImporter::loadShader(record->source);
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

        ComputeShaderAsset asset = AssetImporter::loadComputeShader(record->source);
        m_computeShaderAssets.emplace(id, std::move(asset));
    }

    TextureAsset& AssetManager::getMutableTextureAsset(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get texture from invalid UUID");

        const auto it = m_textureAssets.find(id);

        AIKO_ASSERT(it != m_textureAssets.end(), "Mutable texture asset requested before loading");
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

    string AssetManager::getAssetSource(const AssetId& id, AssetType expectedType) const
    {
        AIKO_ASSERT(id != InvalidAssetId, "Attempting to get source from invalid asset id");
        const AssetRecord* record = m_registry.find(id);
        AIKO_ASSERT(record != nullptr, "Asset id not registered");
        AIKO_ASSERT(record->type == expectedType, "Asset type mismatch");
        return record->source;
    }

    void AssetManager::installTextureAsset(const AssetId& id, TextureAsset asset)
    {
        m_textureAssets.insert_or_assign(id, std::move(asset));
    }

    void AssetManager::installMeshAsset(const AssetId& id, MeshAsset asset)
    {
        m_meshAssets.insert_or_assign(id, std::move(asset));
    }

    void AssetManager::installModelAsset(const AssetId& id, ModelAsset asset)
    {
        m_modelAssets.insert_or_assign(id, std::move(asset));
    }

    void AssetManager::installShaderAsset(const AssetId& id, ShaderAsset asset)
    {
        m_shaderAssets.insert_or_assign(id, std::move(asset));
    }

    void AssetManager::installComputeShaderAsset(const AssetId& id, ComputeShaderAsset asset)
    {
        m_computeShaderAssets.insert_or_assign(
            id,
            std::move(asset)
        );
    }

}
