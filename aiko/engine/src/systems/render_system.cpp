#include "render_system.h"

#include <aiko_types.h>

#include "modules/module_connector.h"
#include "systems/asset_system.h"
#include "modules/render_module.h"
#include "systems/system_connector.h"
#include "systems/scene_system.h"
#include "components/mesh_component.h"
#include "components/sprite_component.h"
#include "components/model_component.h"
#include "assets/types/mesh_asset.h"
#include "types/builtin_shaders.h"
#include <intrumentor/profiler.h>

namespace aiko
{

    RenderSystem::RenderSystem()
    {
    }

    void RenderSystem::init()
    {
        m_materialPrimitives.m_shaderId = m_assetSystem->registerAsset<ShaderAsset>(renderer::BuiltinShader::Model);
        m_assetSystem->loadAsset<ShaderAsset>(m_materialPrimitives.m_shaderId);

        const AssetId uiShader = m_assetSystem->registerAsset<ShaderAsset>(renderer::BuiltinShader::UI);

        m_assetSystem->loadAsset<ShaderAsset>(uiShader);

        m_renderModule->setUiShader(uiShader);

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
        Mesh& mesh = m_renderModule->getMesh(meshId);
        render(trans, mesh, material);
    }

    void RenderSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule)
        BIND_SYSTEM_REQUIRED(AssetSystem, systemConnector, m_assetSystem)
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem)
    }

    void RenderSystem::clearCaches()
    {

    }

    ImguiTextureId RenderSystem::getTargetTextureId() const
    {
        return m_renderModule->getTargetTextureId();
    }

    void RenderSystem::drawUiRect(const vec2& position, const vec2& size, Color color)
    {
        m_renderModule->drawUiRect(position, size, color);
    }

    void RenderSystem::drawUiImage(AssetId textureId, const vec2& position, const vec2& size, Color tint)
    {
        m_renderModule->drawUiImage(textureId, position, size, tint);
    }

    void RenderSystem::drawUiImage(AssetId textureId, const TextureRegion& region, const vec2& position, const vec2& size, Color tint)
    {
        m_renderModule->drawUiImage(textureId, region, position, size, tint);
    }

    void RenderSystem::render(const Transform& trans, const Mesh& mesh, const Material& mat)
    {
        AIKO_FUNCTION_PROFILE
        m_renderModule->submit(trans, mesh, mat);
    }

    void RenderSystem::render(const Transform& trans, const Model& model)
    {
        AIKO_FUNCTION_PROFILE
        for (const auto& submesh : model.getSubMeshes())
        {
            AIKO_ASSERT(submesh.meshId != InvalidAssetId, "Runtime model submesh has invalid mesh id");
            Mesh& mesh = m_renderModule->getMesh(submesh.meshId);
            m_renderModule->submit(trans, mesh, submesh.material);
        }
    }

    void RenderSystem::render(const Transform& trans, const MeshComponent& meshComponent)
    {
        AIKO_FUNCTION_PROFILE
        const AssetId& meshId = meshComponent.getMeshId();
        if (meshId == InvalidAssetId)
        {
            return;
        }
        const Material& material = meshComponent.getMaterial();
        if (material.m_shaderId == InvalidAssetId)
        {
            return;
        }
        if (!m_assetSystem->isLoaded<ShaderAsset>(material.m_shaderId))
        {
            return;
        }
        Mesh& mesh = m_renderModule->getMesh(meshId);
        m_renderModule->submit(trans, mesh, meshComponent.getMaterial());
    }

    void RenderSystem::render(const Transform& trans, const ModelComponent& modelComponent)
    {
        AIKO_FUNCTION_PROFILE
        const AssetId& modelId = modelComponent.getModelId();
        if (modelId == InvalidAssetId)
        {
            return;
        }
        Model& runtimeModel = m_renderModule->getModel(modelId);
        render(trans, runtimeModel);
    }

    void RenderSystem::render(const Transform& trans, const SpriteComponent& spriteComponent)
    {
        AIKO_FUNCTION_PROFILE

        const AssetId& textureId = spriteComponent.getTextureId();
        if (textureId == InvalidAssetId)
        {
            return;
        }

        const Material& material = spriteComponent.getMaterial();
        if (material.m_shaderId == InvalidAssetId)
        {
            return;
        }

        if (m_assetSystem->isLoaded<ShaderAsset>(material.m_shaderId) == false)
        {
            return;
        }

        const TextureRegion& region = spriteComponent.getTextureRegion();

        MeshAsset quad;

        quad.m_vertices =
        {
            { 0.5f,  0.5f, 0.0f},
            { 0.5f, -0.5f, 0.0f},
            {-0.5f, -0.5f, 0.0f},
            {-0.5f,  0.5f, 0.0f},
        };

        quad.m_textCoord =
        {
            {region.max.x, region.max.y},
            {region.max.x, region.min.y},
            {region.min.x, region.min.y},
            {region.min.x, region.max.y},
        };

        quad.m_normals =
        {
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
        };

        quad.m_colors =
        {
            WHITE,
            WHITE,
            WHITE,
            WHITE,
        };

        quad.m_indices =
        {
            0, 1, 3,
            1, 2, 3
        };

        m_renderModule->submitTransient(trans, material, quad, TransientTopology::Triangles);
    }

    void RenderSystem::drawVerticesGpu(const GpuVertexDrawDesc& desc)
    {
        m_renderModule->drawVerticesGpu(desc);
    }

    void RenderSystem::renderInstanced(const Mesh& mesh, const Material& material, const InstanceData* instances, u32 instanceCount)
    {
        AIKO_ASSERT(instances != nullptr, "Instanced render has no instance data");
        AIKO_ASSERT(instanceCount > 0, "Instanced render has zero instances");
        m_renderModule->submitInstanced(mesh, material, instances, instanceCount);
    }

    void RenderSystem::dispatch(const ComputePass& pass, const AssetId& shaderId)
    {
        AIKO_FUNCTION_PROFILE
        AIKO_ASSERT(shaderId != InvalidAssetId, "Attempting to dispatch compute with invalid shader id");
        ComputeShader& shader = m_renderModule->getComputeShader(shaderId);
        ComputePass runtimePass = pass;
        runtimePass.shader = &shader;
        m_renderModule->enqueueCompute(runtimePass);
    }

    void RenderSystem::dispatch(const ComputePass& pass, const ComputeShaderComponent& component)
    {
        AIKO_FUNCTION_PROFILE
        dispatch(pass, component.getShaderId());
    }

    void RenderSystem::requestReadback(const ComputeReadbackRequest& req)
    {
        AIKO_FUNCTION_PROFILE
        m_renderModule->requestReadback(req);
    }

    bool RenderSystem::pollReadback(ComputeReadbackResult& req)
    {
        AIKO_FUNCTION_PROFILE
        return m_renderModule->pollReadback(req);
    }

    void RenderSystem::renderToTarget(const Camera& camera, RenderTarget& target)
    {
        m_renderModule->renderToTarget(camera, target);
    }

    const FrameBuffer& RenderSystem::getTargetTexture() const
    {
        return m_renderModule->getTargetTexture();
    }

    Camera* RenderSystem::getMainCamera()
    {
        return m_sceneSystem->getMainCamera();
    }
}
