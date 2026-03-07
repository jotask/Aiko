#include "asset_manager.h"

#include "asset_importer.h"

namespace aiko
{

    const TextureAsset& AssetManager::getTextureAsset(const AssetId& id)
    {
        auto it = m_textureAssets.find(id);
        if (it != m_textureAssets.end())
        {
            return it->second;
        }
        auto pathIt = m_texturePaths.find(id);
        AIKO_ASSERT(pathIt != m_texturePaths.end(), "Texture asset id not registered");
        TextureAsset asset = AssetImporter::loadTexture(pathIt->second, this);
        auto [insertedIt, _] = m_textureAssets.emplace(id, std::move(asset));
        return insertedIt->second;
    }

    const MeshAsset& AssetManager::getMeshAsset(const AssetId& id)
    {
        auto it = m_meshAssets.find(id);
        if (it != m_meshAssets.end())
        {
            return it->second;
        }

        auto pathIt = m_meshPaths.find(id);
        AIKO_ASSERT(pathIt != m_meshPaths.end(), "Mesh asset id not registered");

        MeshAsset asset = AssetImporter::loadMesh(pathIt->second, this);
        auto [insertedIt, _] = m_meshAssets.emplace(id, std::move(asset));
        return insertedIt->second;
    }

    const ModelAsset& AssetManager::getModelAsset(const AssetId& id)
    {
        auto it = m_modelAssets.find(id);
        if (it != m_modelAssets.end())
        {
            return it->second;
        }

        auto pathIt = m_modelPaths.find(id);
        AIKO_ASSERT(pathIt != m_modelPaths.end(), "Model asset id not registered");

        ModelAsset asset = AssetImporter::loadModel(pathIt->second, this);
        auto [insertedIt, _] = m_modelAssets.emplace(id, std::move(asset));
        return insertedIt->second;
    }

    const ShaderAsset& AssetManager::getShaderAsset(const AssetId& id)
    {
        auto it = m_shaderAssets.find(id);
        if (it != m_shaderAssets.end())
        {
            return it->second;
        }

        auto pathIt = m_shaderPaths.find(id);
        AIKO_ASSERT(pathIt != m_shaderPaths.end(), "Shader asset id not registered");
        AIKO_ASSERT(false, "Shader asset loading is not implemented yet");

        static ShaderAsset dummy{};
        return dummy;
    }

    AssetId AssetManager::registerTexture(std::string_view path)
    {
        AssetId id;
        m_texturePaths[id] = string(path);
        return id;
    }

    AssetId AssetManager::registerMesh(std::string_view path)
    {
        AssetId id;
        m_meshPaths[id] = string(path);
        return id;
    }

    AssetId AssetManager::registerMesh(const MeshAsset& asset)
    {
        AssetId id;
        m_meshAssets.emplace(id, asset);
        return id;
    }

    AssetId AssetManager::registerModel(std::string_view path)
    {
        AssetId id;
        m_modelPaths[id] = string(path);
        return id;
    }

    AssetId AssetManager::registerShader(std::string_view path)
    {
        AssetId id;
        m_shaderPaths[id] = string(path);
        return id;
    }

    bool AssetManager::hasTextureAsset(const AssetId& id) const
    {
        return m_textureAssets.find(id) != m_textureAssets.end() || m_texturePaths.find(id) != m_texturePaths.end() ;
    }

    bool AssetManager::hasMeshAsset(const AssetId& id) const
    {
        return m_meshAssets.find(id) != m_meshAssets.end() || m_meshPaths.find(id) != m_meshPaths.end() ;
    }

    bool AssetManager::hasModelAsset(const AssetId& id) const
    {
        return m_modelAssets.find(id) != m_modelAssets.end() || m_modelPaths.find(id) != m_modelPaths.end() ;
    }

    bool AssetManager::hasShaderAsset(const AssetId& id) const
    {
        return m_shaderAssets.find(id) != m_shaderAssets.end() || m_shaderPaths.find(id) != m_shaderPaths.end() ;
    }

    void AssetManager::clear()
    {
        m_textureAssets.clear();
        m_meshAssets.clear();
        m_modelAssets.clear();
        m_shaderAssets.clear();
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
        m_shaderAssets.erase(id);
    }
}
