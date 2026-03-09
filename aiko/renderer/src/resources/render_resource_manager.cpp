#include "render_resource_manager.h"

namespace aiko
{

    RenderResourceManager::RenderResourceManager(IAssetProvider& assets)
        : m_assetProvider(assets)
    {
    }

    Texture& RenderResourceManager::getTexture(const AssetId& id)
    {
        auto it = m_textures.find(id);
        if (it != m_textures.end())
        {
            return *it->second;
        }
        const TextureAsset& asset = m_assetProvider.getTextureAsset(id);

        auto texture = std::make_unique<Texture>();
        texture->upload(asset);

        Texture& ref = *texture;
        m_textures.emplace(id, std::move(texture));
        return ref;
    }

    Mesh& RenderResourceManager::getMesh(const AssetId& id)
    {

        auto it = m_meshes.find(id);
        if (it != m_meshes.end())
        {
            return *it->second;
        }
        const MeshAsset& asset = m_assetProvider.getMeshAsset(id);

        auto mesh = std::make_unique<Mesh>();
        mesh->upload(asset);

        Mesh& ref = *mesh;
        m_meshes.emplace(id, std::move(mesh));
        return ref;
    }

    Model& RenderResourceManager::getModel(const AssetId& id)
    {
        auto it = m_models.find(id);
        if (it != m_models.end())
        {
            return *it->second;
        }
        const ModelAsset& asset = m_assetProvider.getModelAsset(id);

        auto model = std::make_unique<Model>();
        model->upload(asset);

        Model& ref = *model;
        m_models.emplace(id, std::move(model));
        return ref;
    }

    Shader& RenderResourceManager::getShader(const AssetId& id)
    {
        auto it = m_shaders.find(id);
        if (it != m_shaders.end())
        {
            return *it->second;
        }
        const ShaderAsset& asset = m_assetProvider.getShaderAsset(id);

        auto shader = std::make_unique<Shader>();
        shader->load(asset.vertexPath.c_str(), asset.fragmentPath.c_str());

        Shader& ref = *shader;
        m_shaders.emplace(id, std::move(shader));
        return ref;
    }

    bool RenderResourceManager::hasTexture(const AssetId& id) const
    {
        return m_textures.find(id) != m_textures.end();
    }

    bool RenderResourceManager::hasMesh(const AssetId& id) const
    {
        return m_meshes.find(id) != m_meshes.end();
    }

    bool RenderResourceManager::hasModel(const AssetId& id) const
    {
        return m_models.find(id) != m_models.end();
    }

    bool RenderResourceManager::hasShader(const AssetId& id) const
    {
        return m_shaders.find(id) != m_shaders.end();
    }

    void RenderResourceManager::updateTexture(const AssetId& id)
    {
        auto it = m_textures.find(id);
        if (it == m_textures.end())
        {
            return;
        }
        const TextureAsset& asset = m_assetProvider.getTextureAsset(id);
        it->second->update(asset);
    }

    void RenderResourceManager::unloadTexture(const AssetId& id)
    {
        auto it = m_textures.find(id);
        if (it != m_textures.end())
        {
            it->second->unload();
            m_textures.erase(it);
        }
    }

    void RenderResourceManager::unloadMesh(const AssetId& id)
    {
        auto it = m_meshes.find(id);
        if (it != m_meshes.end())
        {
            it->second->unload();
            m_meshes.erase(it);
        }
    }

    void RenderResourceManager::unloadModel(const AssetId& id)
    {
        auto it = m_models.find(id);
        if (it != m_models.end())
        {
            it->second->unload();
            m_models.erase(it);
        }
    }

    void RenderResourceManager::unloadShader(const AssetId& id)
    {
        auto it = m_shaders.find(id);
        if (it != m_shaders.end())
        {
            it->second->unload();
            m_shaders.erase(it);
        }
    }

    void RenderResourceManager::clear()
    {
        for (auto& texture : m_textures)
        {
            if (texture.second != nullptr)
            {
                texture.second->unload();
            }
        }
        for (auto& mesh : m_meshes)
        {
            if (mesh.second != nullptr)
            {
                mesh.second->unload();
            }
        }
        for (auto& model : m_models)
        {
            if (model.second != nullptr)
            {
                model.second->unload();
            }
        }
        for (auto& shader : m_shaders)
        {
            if (shader.second != nullptr)
            {
                shader.second->unload();
            }
        }
        m_textures.clear();
        m_meshes.clear();
        m_models.clear();
        m_shaders.clear();
    }
}
