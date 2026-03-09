#include "render_system.h"

#include <stdexcept>

#include <aiko_types.h>

#include "modules/module_connector.h"
#include "modules/assets_manager_module.h"
#include "modules/render_module.h"
#include "systems/system_connector.h"
#include "systems/scene_system.h"
#include "components/mesh_component.h"
#include "components/sprite_component.h"
#include "components/model_component.h"
#include "resolver/material_resolver.h"

namespace aiko
{

    RenderSystem::RenderSystem()
    {
    }

    void RenderSystem::init()
    {
        AssetId shaderId = m_assetManagerModule->getManager()->registerShader("model");
        m_materialPrimitives.m_shader = &m_renderModule->getRenderer().resources().getShader(shaderId);
        m_materialPrimitives.m_lit = true;
        m_materialPrimitives.m_useVertexColor = false;
        m_materialPrimitives.m_diffuseTexture = nullptr;
    }

    void RenderSystem::update()
    {

    }

    void RenderSystem::render()
    {

    }

    void RenderSystem::dispose()
    {
        BaseSystem::dispose();
        clearCaches();
    }

    void RenderSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule)
        BIND_MODULE_REQUIRED(AssetsManagerModule, moduleConnector, m_assetManagerModule)
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem)
    }

    void RenderSystem::clearCaches()
    {
        m_modelSubmeshCache.clear();
        m_materialCache.clear();
    }

    void RenderSystem::render(const Transform& trans, const Mesh& mesh, const Material& mat)
    {
        m_renderModule->getRenderer().submit(trans, mesh, mat);
    }

    void RenderSystem::render(const Transform& trans, const MeshComponent& meshComponent)
    {
        const AssetId& meshId = meshComponent.getMeshId();
        AIKO_ASSERT(meshId != InvalidAssetId, "MeshComponent has no mesh id assigned");
        Mesh& mesh = m_renderModule->getRenderer().resources().getMesh(meshId);
        Material& runtimeMaterial = resolveCachedMaterial( meshComponent.getMaterial(), meshComponent.getMaterialInstance());
        m_renderModule->getRenderer().submit(trans, mesh, runtimeMaterial);
    }

    void RenderSystem::render(const Transform& trans, const ModelComponent& modelComponent)
    {
        const AssetId& modelId = modelComponent.getModelId();
        AIKO_ASSERT(modelId != InvalidAssetId, "ModelComponent has no model id assigned");

        const ModelAsset& modelAsset = m_assetManagerModule->getManager()->getModelAsset(modelId);

        for (size_t i = 0; i < modelAsset.submeshes.size(); ++i)
        {
            const auto& submesh = modelAsset.submeshes[i];

            const string key = modelId.get() + ":" + std::to_string(i);

            Mesh* mesh = nullptr;
            auto it = m_modelSubmeshCache.find(key);
            if (it == m_modelSubmeshCache.end())
            {
                auto runtimeMesh = std::make_unique<Mesh>();
                runtimeMesh->upload(submesh.mesh);
                mesh = runtimeMesh.get();
                m_modelSubmeshCache.emplace(key, std::move(runtimeMesh));
            }
            else
            {
                mesh = it->second.get();
            }
            Material& runtimeMaterial = resolveCachedMaterial(submesh.material, modelComponent.getMaterialInstance());
            m_renderModule->getRenderer().submit(trans, *mesh, runtimeMaterial);
        }
    }

    void RenderSystem::render(const Transform& trans, const SpriteComponent& spriteComponent)
    {
        const AssetId& meshId = spriteComponent.getMeshId();
        AIKO_ASSERT(meshId != InvalidAssetId, "SpriteComponent has no mesh id assigned");
        Mesh& mesh = m_renderModule->getRenderer().resources().getMesh(meshId);
        Material& runtimeMaterial = resolveCachedMaterial( spriteComponent.getMaterial(), spriteComponent.getMaterialInstance());
        m_renderModule->getRenderer().submit(trans, mesh, runtimeMaterial);
    }

    const FrameBuffer& RenderSystem::getTargetTexture() const
    {
        return m_renderModule->getRenderer().getTargetTexture();
    }

    Camera* RenderSystem::getMainCamera()
    {
        return m_sceneSystem->getMainCamera();
    }

    static string buildMaterialKey(const MaterialAsset& material, const MaterialInstance& instance)
    {
        const AssetId shaderId = instance.shaderId != InvalidAssetId ? instance.shaderId : material.shaderId;

        return shaderId.get() + "|" +
               material.diffuseTextureId.get() + "|" +
               std::to_string(material.useVertexColor) + "|" +
               std::to_string(material.lit) + "|" +
               std::to_string(material.baseColor.rgba());
    }

    Material& RenderSystem::resolveCachedMaterial(const MaterialAsset& materialAsset, const MaterialInstance& materialInstance)
    {
        const string key = buildMaterialKey(materialAsset, materialInstance);

        auto it = m_materialCache.find(key);
        if (it != m_materialCache.end())
        {
            return *it->second.material;
        }

        auto material = MaterialResolver::resolve( materialAsset, materialInstance, *m_assetManagerModule->getManager(), m_renderModule->getRenderer());

        Material& ref = *material;

        CachedMaterialEntry entry{};
        entry.textureId = materialAsset.diffuseTextureId;
        entry.material = std::move(material);

        m_materialCache.emplace(key, std::move(entry));
        return ref;

    }

}
