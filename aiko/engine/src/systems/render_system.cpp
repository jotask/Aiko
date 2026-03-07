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
        m_materialPrimitives.m_shader.load("model");
        m_materialPrimitives.m_lit = true;
    }

    void RenderSystem::update()
    {

    }

    void RenderSystem::render()
    {
        m_frameMeshes.clear();
        m_frameMaterials.clear();
    }

    void RenderSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_MODULE_REQUIRED(RenderModule, moduleConnector, m_renderModule)
        BIND_MODULE_REQUIRED(AssetsManagerModule, moduleConnector, m_assetManagerModule)
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem)
    }

    void RenderSystem::render(const Transform& trans, const Mesh& mesh, const Material& mat)
    {
        m_renderModule->getRenderer().submit(trans, mesh, mat);
    }

    void RenderSystem::render(const Transform& trans, const MeshComponent& meshComponent)
    {
        const AssetId& meshId = meshComponent.getMeshId();
        Mesh& mesh = m_renderModule->getRenderer().resources().getMesh(meshId);
        m_frameMaterials.push_back(MaterialResolver::resolve( meshComponent.getMaterial(), meshComponent.getMaterialInstance(), m_renderModule->getRenderer()));
        Material& runtimeMaterial = m_frameMaterials.back();
        m_renderModule->getRenderer().submit(trans, mesh, runtimeMaterial);
    }

    void RenderSystem::render(const Transform& trans, const ModelComponent& modelComponent)
    {
        const AssetId& modelId = modelComponent.getModelId();
        const ModelAsset& modelAsset = m_assetManagerModule->getManager()->getModelAsset(modelId);

        for (const auto& submesh : modelAsset.submeshes)
        {
            m_frameMeshes.emplace_back();
            Mesh& mesh = m_frameMeshes.back();
            mesh.upload(submesh.mesh);

            m_frameMaterials.push_back(MaterialResolver::resolve(submesh.material, m_renderModule->getRenderer()));
            Material& runtimeMaterial = m_frameMaterials.back();

            m_renderModule->getRenderer().submit(trans, mesh, runtimeMaterial);
        }
    }

    void RenderSystem::render(const Transform& trans, const SpriteComponent& spriteComponent)
    {
        const AssetId& meshId = spriteComponent.getMeshId();
        Mesh& mesh = m_renderModule->getRenderer().resources().getMesh(meshId);
        m_frameMaterials.push_back(MaterialResolver::resolve(spriteComponent.getMaterial(), m_renderModule->getRenderer()));
        Material& runtimeMaterial = m_frameMaterials.back();
        m_renderModule->getRenderer().submit(trans, mesh, runtimeMaterial);
    }

    FrameBuffer RenderSystem::getTargetTexture() const
    {
        return m_renderModule->getRenderer().getTargetTexture();
    }

    Camera* RenderSystem::getMainCamera()
    {
        return m_sceneSystem->getMainCamera();
    }

}
