#include "material_resolver.h"

#include "aiko_renderer.h"
#include "../../../engine/src/assets/asset_manager.h"

namespace aiko
{

    AikoUPtr<Material> MaterialResolver::resolve(const MaterialAsset& materialAsset, IAssetProvider& assets, AikoRenderer& renderer)
    {

        auto material = std::make_unique<Material>();

        material->m_useVertexColor = materialAsset.useVertexColor;
        material->m_lit = materialAsset.lit;
        material->m_baseColor = materialAsset.baseColor;

        if (materialAsset.shaderId != InvalidAssetId)
        {
            material->m_shader = &renderer.resources().getShader(materialAsset.shaderId);
        }
        else
        {
            static AssetId s_defaultModelShaderId = InvalidAssetId;
            if (s_defaultModelShaderId == InvalidAssetId)
            {
                // This assumes the asset provider is AssetManager and path registration is deduplicated.
                AssetManager* assetManager = dynamic_cast<AssetManager*>(&assets);
                AIKO_ASSERT(assetManager != nullptr, "MaterialResolver requires AssetManager for default shader registration");
                s_defaultModelShaderId = assetManager->registerShader("model");
            }
            material->m_shader = &renderer.resources().getShader(s_defaultModelShaderId);
        }

        if (materialAsset.diffuseTextureId != InvalidAssetId)
        {
            material->m_diffuseTexture = &renderer.resources().getTexture(materialAsset.diffuseTextureId);
        }

        return material;
    }

    AikoUPtr<Material> MaterialResolver::resolve(const MaterialAsset& asset, const MaterialInstance& instance, IAssetProvider& assets, AikoRenderer& renderer)
    {
        MaterialAsset effective = asset;

        if (instance.shaderId != InvalidAssetId)
        {
            effective.shaderId = instance.shaderId;
        }

        return resolve(effective, assets, renderer);
    }

}
