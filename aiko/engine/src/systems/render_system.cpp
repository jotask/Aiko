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
#include <intrumentor/profiler.h>
#include "resolver/material_resolver.h"

namespace aiko
{

    RenderSystem::RenderSystem()
    {
    }

    void RenderSystem::init()
    {
        m_materialPrimitives.m_shaderId = m_assetManagerModule->getManager()->registerShader("model");
        m_materialPrimitives.m_lit = false;
        m_materialPrimitives.m_useVertexColor = false;
        m_materialPrimitives.m_baseColor = MAGENTA;

        m_primitiveMeshCache.init();
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
        m_materialCache.clear();
    }

    void RenderSystem::render(const Transform& trans, const Mesh& mesh, const Material& mat)
    {
        AIKO_ZONE_SCOPED
        m_renderModule->getRenderer().submit(trans, mesh, mat);
    }

    void RenderSystem::render(const Transform& trans, const Model& model)
    {
        AIKO_ZONE_SCOPED
        const MaterialInstance defaultInstance{};
        render(trans, model, defaultInstance);
    }

    void RenderSystem::render(const Transform& trans, const Model& model, const MaterialInstance& instance)
    {
        AIKO_ZONE_SCOPED
        for (const auto& submesh : model.getSubMeshes())
        {
            AIKO_ASSERT(submesh.meshId != InvalidAssetId, "Runtime model submesh has invalid mesh id");
            AIKO_ASSERT(submesh.material != nullptr, "Runtime model submesh has null material");

            Mesh& mesh = m_renderModule->getRenderer().resources().getMesh(submesh.meshId);
            Material& runtimeMaterial = resolveCachedMaterial(*submesh.material, instance);

            m_renderModule->getRenderer().submit(trans, mesh, runtimeMaterial);
        }
    }

    void RenderSystem::render(const Transform& trans, const MeshComponent& meshComponent)
    {
        AIKO_ZONE_SCOPED
        const AssetId& meshId = meshComponent.getMeshId();
        AIKO_ASSERT(meshId != InvalidAssetId, "MeshComponent has no mesh id assigned");
        Mesh& mesh = m_renderModule->getRenderer().resources().getMesh(meshId);
        Material& runtimeMaterial = resolveCachedMaterial( meshComponent.getMaterial(), meshComponent.getMaterialInstance());
        m_renderModule->getRenderer().submit(trans, mesh, runtimeMaterial);
    }

    void RenderSystem::render(const Transform& trans, const ModelComponent& modelComponent)
    {
        AIKO_ZONE_SCOPED
        const AssetId& modelId = modelComponent.getModelId();
        AIKO_ASSERT(modelId != InvalidAssetId, "ModelComponent has no model id assigned");

        Model& runtimeModel = m_renderModule->getRenderer().resources().getModel(modelId);
        render(trans, runtimeModel, modelComponent.getMaterialInstance());

    }

    void RenderSystem::render(const Transform& trans, const SpriteComponent& spriteComponent)
    {
        AIKO_ZONE_SCOPED
        const AssetId& meshId = spriteComponent.getMeshId();
        AIKO_ASSERT(meshId != InvalidAssetId, "SpriteComponent has no mesh id assigned");
        Mesh& mesh = m_renderModule->getRenderer().resources().getMesh(meshId);
        Material& runtimeMaterial = resolveCachedMaterial( spriteComponent.getMaterial(), spriteComponent.getMaterialInstance());
        m_renderModule->getRenderer().submit(trans, mesh, runtimeMaterial);
    }

    void RenderSystem::dispatch(const ComputePass& pass, const AssetId& shaderId)
    {
        AIKO_ZONE_SCOPED
        AIKO_ASSERT(shaderId != InvalidAssetId, "Attempting to dispatch compute with invalid shader id");
        ComputeShader& shader = m_renderModule->getRenderer().resources().getComputeShader(shaderId);
        ComputePass runtimePass = pass;
        runtimePass.shader = &shader;
        m_renderModule->getRenderer().enqueueCompute(runtimePass);
    }

    void RenderSystem::dispatch(const ComputePass& pass, const ComputeShaderComponent& component)
    {
        AIKO_ZONE_SCOPED
        dispatch(pass, component.getShaderId());
    }

    void RenderSystem::requestReadback(const ComputeReadbackRequest& req)
    {
        AIKO_ZONE_SCOPED
        m_renderModule->getRenderer().requestReadback(req);
    }

    bool RenderSystem::pollReadback(ComputeReadbackResult& req)
    {
        AIKO_ZONE_SCOPED
        return m_renderModule->getRenderer().pollReadback(req);
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
        AIKO_ZONE_SCOPED
        const AssetId& shaderId = instance.shaderId != InvalidAssetId ? instance.shaderId : material.shaderId;

        const uint64_t runtimeTextureKey =
            instance.runtimeDiffuseTexture != nullptr
                ? reinterpret_cast<uint64_t>(instance.runtimeDiffuseTexture)
                : 0ull;

        return shaderId.get() + "|" +
               material.diffuseTextureId.get() + "|" +
               std::to_string(material.useVertexColor) + "|" +
               std::to_string(material.lit) + "|" +
               std::to_string(material.baseColor.rgba()) + "|" +
               std::to_string(runtimeTextureKey);
    }

    Material& RenderSystem::resolveCachedMaterial(const MaterialAsset& materialAsset, const MaterialInstance& materialInstance)
    {
        AIKO_ZONE_SCOPED
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
