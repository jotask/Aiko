#include "asset_binding_system.h"

#include "assets/types/mesh_asset.h"

#include "components/model_component.h"

#include "scene/scene.h"

#include "systems/asset_system.h"
#include "systems/scene_system.h"
#include "systems/system_connector.h"
#include "components/compute_shader_component.h"
#include "assets/types/shader_asset.h"
#include "components/mesh_component.h"
#include "models/mesh_factory.h"
#include "components/sprite_component.h"
#include "assets/types/texture_asset.h"
#include "assets/asset_binding.h"

namespace aiko
{
    void AssetBindingSystem::connect(ModuleConnector* moduleConnector, SystemConnector* systemConnector)
    {
        BIND_SYSTEM_REQUIRED(AssetSystem, systemConnector, m_assetSystem);
        BIND_SYSTEM_REQUIRED(SceneSystem, systemConnector, m_sceneSystem);
    }

    void AssetBindingSystem::update()
    {
        BaseSystem::update();

        AssetBindingContext context(*m_assetSystem);

        Scene& scene = m_sceneSystem->getScene();

        for (MeshComponent* component : scene.components<MeshComponent>())
        {
            if (component != nullptr)
            {
                resolveMesh(*component);
            }
        }

        for (GameObject* object : scene.consumeAssetBindingDirtyObjects())
        {
            if (object == nullptr)
            {
                continue;
            }

            for (Component* component : object->getComponents())
            {
                if (auto* binding = dynamic_cast<IAssetBinding*>(component))
                {
                    binding->resolveAssetBinding(context);
                }
            }
        }

        for (SpriteComponent* component : scene.components<SpriteComponent>())
        {
            if (component != nullptr)
            {
                resolveSprite(*component);
            }
        }

        for (ComputeShaderComponent* component : scene.components<ComputeShaderComponent>())
        {
            if (component != nullptr)
            {
                resolveComputeShader(*component);
            }
        }
    }

    void AssetBindingSystem::resolveMesh(MeshComponent& component)
    {
        AssetReference<MeshAsset>& reference = component.meshReference();
        if (reference.isRequested())
        {
            reference.markLoading();
            const AssetId id = m_assetSystem->load<MeshAsset>(reference.source());
            if (id == InvalidAssetId)
            {
                reference.fail();
            }
            else
            {
                reference.resolve(id);
            }
        }

        if (component.hasCubeRequest())
        {
            const AssetId meshId = m_assetSystem->create(mesh::factory::generateCube());
            component.setMeshId(meshId);
            component.getMaterial().shaderId = m_assetSystem->load<ShaderAsset>("model");
            component.clearPrimitiveRequest();
        }
    }

    void AssetBindingSystem::resolveSprite(SpriteComponent& component)
    {
        AssetReference<TextureAsset>& texture = component.textureReference();

        if (texture.isRequested())
        {
            texture.markLoading();

            const AssetId textureId = m_assetSystem->load<TextureAsset>(texture.source());

            if (textureId == InvalidAssetId)
            {
                texture.fail();
            }
            else
            {
                texture.resolve(textureId);
                component.getMaterial().diffuseTextureId = textureId;
            }

            if (component.getMeshId() == InvalidAssetId)
            {
                component.setMeshId(m_assetSystem->create(mesh::factory::generateQuad()));
            }

            component.getMaterial().shaderId = m_assetSystem->load<ShaderAsset>("model");
        }

        if (component.hasCreateRequest())
        {
            if (component.getMeshId() == InvalidAssetId)
            {
                component.setMeshId(m_assetSystem->create(mesh::factory::generateQuad()));
            }

            component.getMaterial().shaderId = m_assetSystem->load<ShaderAsset>("model");

            TextureAsset textureAsset{};
            textureAsset.desc.type = TextureType::Sampled;
            textureAsset.desc.format = TextureFormat::RGBA8;
            textureAsset.desc.width = static_cast<uint>(component.getWidth());
            textureAsset.desc.height = static_cast<uint>(component.getHeight());
            textureAsset.desc.mipmaps = 1;
            textureAsset.desc.computeWrite = false;
            textureAsset.pixels = component.getPixels();

            const AssetId textureId = m_assetSystem->create(textureAsset);

            component.setTextureId(textureId);

            component.clearCreateRequest();
            component.clearDirty();
        }

        if (component.hasRefreshRequest())
        {
            const AssetId& textureId = component.getTextureId();

            if (textureId == InvalidAssetId)
            {
                return;
            }

            TextureAsset& textureAsset = m_assetSystem->getMutableTextureAsset(textureId);

            textureAsset.pixels = component.getPixels();

            textureAsset.desc.width = static_cast<uint>(component.getWidth());

            textureAsset.desc.height = static_cast<uint>(component.getHeight());

            m_assetSystem->invalidateTexture(textureId);

            component.clearDirty();
            component.clearRefreshRequest();
        }
    }

    void AssetBindingSystem::resolveComputeShader(ComputeShaderComponent& component)
    {
        AssetReference<ComputeShaderAsset>& reference = component.shaderReference();
        if (reference.isRequested() == false)
        {
            return;
        }
        reference.markLoading();
        const AssetId id = m_assetSystem->load<ComputeShaderAsset>(reference.source());
        if (id == InvalidAssetId)
        {
            reference.fail();
            return;
        }
        reference.resolve(id);
    }
}
