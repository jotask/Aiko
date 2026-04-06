#include "render_system.h"

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

    void RenderSystem::submitMesh(const Transform& trans, const AssetId& meshId, const Material& material)
    {
        AIKO_FUNCTION_PROFILE
        AIKO_ASSERT(meshId != InvalidAssetId, "MeshComponent has no mesh id assigned");
        Mesh& mesh = m_renderModule->getRenderer().resources().getMesh(meshId);
        render(trans, mesh, material);
    }

    void RenderSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule)
        BIND_MODULE_REQUIRED(AssetsManagerModule, moduleConnector, m_assetManagerModule)
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem)
    }

    void RenderSystem::clearCaches()
    {

    }

    void RenderSystem::render(const Transform& trans, const Mesh& mesh, const Material& mat)
    {
        AIKO_FUNCTION_PROFILE
        m_renderModule->getRenderer().submit(trans, mesh, mat);
    }

    void RenderSystem::render(const Transform& trans, const Model& model)
    {
        AIKO_FUNCTION_PROFILE
        const MaterialInstance defaultInstance{};
        render(trans, model, defaultInstance);
    }

    void RenderSystem::render(const Transform& trans, const Model& model, const MaterialInstance& instance)
    {
        AIKO_FUNCTION_PROFILE
        for (const auto& submesh : model.getSubMeshes())
        {
            AIKO_ASSERT(submesh.meshId != InvalidAssetId, "Runtime model submesh has invalid mesh id");
            AIKO_ASSERT(submesh.material != nullptr, "Runtime model submesh has null material");
            Mesh& mesh = m_renderModule->getRenderer().resources().getMesh(submesh.meshId);
            m_renderModule->getRenderer().submit(trans, mesh, *submesh.material, instance);
        }
    }

    void RenderSystem::render(const Transform& trans, const MeshComponent& meshComponent)
    {
        AIKO_FUNCTION_PROFILE
        const AssetId& meshId = meshComponent.getMeshId();
        AIKO_ASSERT(meshId != InvalidAssetId, "MeshComponent has no mesh id assigned");
        Mesh& mesh = m_renderModule->getRenderer().resources().getMesh(meshId);
        m_renderModule->getRenderer().submit(trans, mesh, meshComponent.getMaterial(), meshComponent.getMaterialInstance());
    }

    void RenderSystem::render(const Transform& trans, const ModelComponent& modelComponent)
    {
        AIKO_FUNCTION_PROFILE
        const AssetId& modelId = modelComponent.getModelId();
        AIKO_ASSERT(modelId != InvalidAssetId, "ModelComponent has no model id assigned");

        Model& runtimeModel = m_renderModule->getRenderer().resources().getModel(modelId);
        render(trans, runtimeModel, modelComponent.getMaterialInstance());

    }

    void RenderSystem::render(const Transform& trans, const SpriteComponent& spriteComponent)
    {
        AIKO_FUNCTION_PROFILE
        const AssetId& meshId = spriteComponent.getMeshId();
        AIKO_ASSERT(meshId != InvalidAssetId, "SpriteComponent has no mesh id assigned");
        Mesh& mesh = m_renderModule->getRenderer().resources().getMesh(meshId);
        m_renderModule->getRenderer().submit(trans, mesh, spriteComponent.getMaterial(), spriteComponent.getMaterialInstance());
    }

    void RenderSystem::dispatch(const ComputePass& pass, const AssetId& shaderId)
    {
        AIKO_FUNCTION_PROFILE
        AIKO_ASSERT(shaderId != InvalidAssetId, "Attempting to dispatch compute with invalid shader id");
        ComputeShader& shader = m_renderModule->getRenderer().resources().getComputeShader(shaderId);
        ComputePass runtimePass = pass;
        runtimePass.shader = &shader;
        m_renderModule->getRenderer().enqueueCompute(runtimePass);
    }

    void RenderSystem::dispatch(const ComputePass& pass, const ComputeShaderComponent& component)
    {
        AIKO_FUNCTION_PROFILE
        dispatch(pass, component.getShaderId());
    }

    void RenderSystem::requestReadback(const ComputeReadbackRequest& req)
    {
        AIKO_FUNCTION_PROFILE
        m_renderModule->getRenderer().requestReadback(req);
    }

    bool RenderSystem::pollReadback(ComputeReadbackResult& req)
    {
        AIKO_FUNCTION_PROFILE
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
}
