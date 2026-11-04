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
        return *acquireMesh(id);
    }

    std::shared_ptr<Mesh> RenderResourceManager::acquireMesh(const AssetId& id)
    {
        const auto it = m_meshes.find(id);

        if (it != m_meshes.end())
        {
            std::shared_ptr<Mesh> mesh = it->second;

            if (!mesh->isValid())
            {
                const MeshAsset& asset = m_assetProvider.getMeshAsset(id);
                mesh->upload(asset);
            }

            return mesh;
        }

        const MeshAsset& asset = m_assetProvider.getMeshAsset(id);

        auto mesh = std::make_shared<Mesh>();
        mesh->upload(asset);

        m_meshes.emplace(id, mesh);

        return mesh;
    }

    Model& RenderResourceManager::requestModel(const AssetId& id)
    {
        AIKO_ASSERT(id != InvalidAssetId, "Cannot request an invalid model asset");

        const auto it = m_models.find(id);

        if (it != m_models.end())
        {
            return *it->second;
        }

        auto model = std::make_unique<Model>();

        Model& reference = *model;

        m_models.emplace(id, std::move(model));

        return reference;
    }

    Model& RenderResourceManager::getModel(const AssetId& id)
    {
        const auto it = m_models.find(id);

        if (it != m_models.end())
        {
            return *it->second;
        }

        Model& model = requestModel(id);

        updateModel(id);

        return model;
    }

    void RenderResourceManager::updateModel(const AssetId& id)
    {
        const auto it = m_models.find(id);

        if (it == m_models.end())
        {
            return;
        }

        const ModelAsset& asset = m_assetProvider.getModelAsset(id);

        Model replacement;

        for (const ModelAsset::SubMesh& submesh : asset.submeshes)
        {
            Material material{};

            material.m_shaderId = submesh.material.shaderId;
            material.m_baseColor = submesh.material.baseColor;
            material.m_useVertexColor = submesh.material.useVertexColor;
            material.m_lit = submesh.material.lit;

            if (submesh.material.diffuseTextureId != InvalidAssetId)
            {
                material.setTexture("u_texture", submesh.material.diffuseTextureId);
            }

            replacement.addSubMesh(acquireMesh(submesh.meshId), std::move(material));
        }

        it->second->replaceContents(std::move(replacement));
    }

    Model RenderResourceManager::getModelValue(const AssetId& id)
    {
        return requestModel(id).share();
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

    void RenderResourceManager::updateMesh(const AssetId& id)
    {
        const auto it = m_meshes.find(id);
        if (it == m_meshes.end())
        {
            return;
        }
        const MeshAsset& asset = m_assetProvider.getMeshAsset(id);
        it->second->upload(asset);
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
        for (auto& [id, model] : m_models)
        {
            if (model)
            {
                model->clearContents();
            }
        }

        m_models.clear();
        m_meshes.clear();

        for (auto& [id, texture] : m_textures)
        {
            if (texture != nullptr)
            {
                texture->unload();
            }
        }

        for (auto& [id, texture] : m_srgbTextures)
        {
            if (texture != nullptr)
            {
                texture->unload();
            }
        }

        for (auto& [id, shader] : m_shaders)
        {
            if (shader != nullptr)
            {
                shader->unload();
            }
        }

        for (auto& [id, shader] : m_computeShaders)
        {
            if (shader != nullptr)
            {
                shader->unload();
            }
        }

        m_textures.clear();
        m_srgbTextures.clear();
        m_shaders.clear();
        m_computeShaders.clear();
    }

}
