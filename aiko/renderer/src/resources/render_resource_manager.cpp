#include "render_resource_manager.h"

#include "models/texture.h"
#include "models/mesh.h"
#include "models/model.h"
#include "models/shader.h"
#include "models/compute_shader.h"
#include <assets/iasset_provider.h>
#include <assets/types/texture_asset.h>

namespace aiko
{

    namespace
    {
        TextureAsset makeLinearTextureAsset(const TextureAsset& source)
        {
            TextureAsset result = source;

            if (result.desc.type == TextureType::Sampled &&
                result.desc.format == TextureFormat::RGBA8_SRGB)
            {
                result.desc.format = TextureFormat::RGBA8;
            }

            return result;
        }
    }

    RenderResourceManager::RenderResourceManager(IAssetProvider& assets)
        : m_assetProvider(assets)
    {
    }

    RenderResourceManager::~RenderResourceManager() = default;


    Texture& RenderResourceManager::getTexture(const AssetId& id)
    {
        auto it = m_textures.find(id);

        if (it != m_textures.end())
        {
            Texture& texture = *it->second;

            if (!texture.isValid())
            {
                const TextureAsset& source = m_assetProvider.getTextureAsset(id);

                texture.upload(makeLinearTextureAsset(source));
            }

            return texture;
        }

        const TextureAsset& source = m_assetProvider.getTextureAsset(id);

        auto texture = std::make_unique<Texture>();

        texture->upload(makeLinearTextureAsset(source));

        Texture& ref = *texture;

        m_textures.emplace(id, std::move(texture));

        return ref;
    }

    Texture& RenderResourceManager::getTexture(const AssetId& id, TextureColorSpace colorSpace)
    {
        if (colorSpace == TextureColorSpace::Linear)
        {
            return getTexture(id);
        }

        AIKO_ASSERT(colorSpace == TextureColorSpace::SRGB, "Unsupported texture color space");

        auto& texture = m_srgbTextures[id];

        if (texture == nullptr)
        {
            texture = std::make_unique<Texture>();
        }

        if (!texture->isValid())
        {
            TextureAsset asset = m_assetProvider.getTextureAsset(id);
            AIKO_ASSERT(asset.desc.type == TextureType::Sampled && (asset.desc.format == TextureFormat::RGBA8 || asset.desc.format == TextureFormat::RGBA8_SRGB), "sRGB variant requires an RGBA8 sampled texture");
            asset.desc.format = TextureFormat::RGBA8_SRGB;
            texture->upload(asset);
        }

        return *texture;
    }

    Texture* RenderResourceManager::tryGetTexture(const AssetId& id)
    {
        if (id == InvalidAssetId || !m_assetProvider.isTextureAssetLoaded(id))
        {
            return nullptr;
        }

        return &getTexture(id);
    }

    Texture* RenderResourceManager::tryGetTexture(const AssetId& id, TextureColorSpace colorSpace)
    {
        if (id == InvalidAssetId || !m_assetProvider.isTextureAssetLoaded(id))
        {
            return nullptr;
        }

        return &getTexture(id, colorSpace);
    }

    Mesh& RenderResourceManager::getMesh(const AssetId& id)
    {
        auto it = m_meshes.find(id);

        if (it != m_meshes.end())
        {
            Mesh& mesh = *it->second;

            if (!mesh.isValid())
            {
                // The previous upload failed.
                // Retry without replacing the cached Mesh object.
                const MeshAsset& asset = m_assetProvider.getMeshAsset(id);
                mesh.upload(asset);
            }

            return mesh;
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

    ComputeShader& RenderResourceManager::getComputeShader(const AssetId& id)
    {
        auto it = m_computeShaders.find(id);
        if (it != m_computeShaders.end())
        {
            return *it->second;
        }
        const ComputeShaderAsset& asset = m_assetProvider.getComputeShaderAsset(id);

        AIKO_ASSERT(asset.computePath.empty() == false, "Shader asset has no compute path");

        auto shader = std::make_unique<ComputeShader>();
        shader->load(asset.computePath);

        ComputeShader& ref = *shader;
        m_computeShaders.emplace(id, std::move(shader));
        return ref;
    }

    bool RenderResourceManager::hasTexture(const AssetId& id) const
    {
        return m_textures.contains(id) || m_srgbTextures.contains(id);
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

    bool RenderResourceManager::hasComputeShader(const AssetId& id) const
    {
        return m_computeShaders.find(id) != m_computeShaders.end();
    }

    void RenderResourceManager::updateTexture(const AssetId& id)
    {
        auto linearIt = m_textures.find(id);
        auto srgbIt = m_srgbTextures.find(id);

        if (linearIt == m_textures.end() && srgbIt == m_srgbTextures.end())
        {
            return;
        }

        const TextureAsset& asset = m_assetProvider.getTextureAsset(id);

        if (linearIt != m_textures.end())
        {
            linearIt->second->update(makeLinearTextureAsset(asset));
        }

        if (srgbIt != m_srgbTextures.end())
        {
            TextureAsset srgbAsset = asset;
            AIKO_ASSERT(srgbAsset.desc.type == TextureType::Sampled && (srgbAsset.desc.format == TextureFormat::RGBA8 || srgbAsset.desc.format == TextureFormat::RGBA8_SRGB), "sRGB variant requires an RGBA8 sampled texture");
            srgbAsset.desc.format = TextureFormat::RGBA8_SRGB;
            srgbIt->second->update(srgbAsset);
        }
    }

    void RenderResourceManager::unloadTexture(const AssetId& id)
    {
        auto it = m_textures.find(id);
        if (it != m_textures.end())
        {
            it->second->unload();
            m_textures.erase(it);
        }

        auto srgbIt = m_srgbTextures.find(id);
        if (srgbIt != m_srgbTextures.end())
        {
            srgbIt->second->unload();
            m_srgbTextures.erase(srgbIt);
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

    void RenderResourceManager::unloadComputeShader(const AssetId& id)
    {
        auto it = m_computeShaders.find(id);
        if (it != m_computeShaders.end())
        {
            it->second->unload();
            m_computeShaders.erase(it);
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
        for (auto& texture: m_srgbTextures)
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
        for (auto& compute : m_computeShaders)
        {
            if (compute.second != nullptr)
            {
                compute.second->unload();
            }
        }
        m_textures.clear();
        m_srgbTextures.clear();
        m_meshes.clear();
        m_models.clear();
        m_shaders.clear();
        m_computeShaders.clear();
    }
}
