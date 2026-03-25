#include "material_resolver.h"

#include "aiko_renderer.h"
#include "../../../engine/src/assets/asset_manager.h"

namespace aiko
{

    AikoUPtr<Material> MaterialResolver::resolve(const MaterialAsset& materialAsset, IAssetProvider& assets, AikoRenderer& renderer)
    {

        AIKO_UNUSED(assets);
        AIKO_UNUSED(renderer);

        auto material = std::make_unique<Material>();

        material->m_useVertexColor = materialAsset.useVertexColor;
        material->m_lit = materialAsset.lit;
        material->m_baseColor = materialAsset.baseColor;

        material->m_shaderId = materialAsset.shaderId;
        material->m_diffuseTextureId = materialAsset.diffuseTextureId;

        return material;
    }

    AikoUPtr<Material> MaterialResolver::resolve(const MaterialAsset& asset, const MaterialInstance& instance, IAssetProvider& assets, AikoRenderer& renderer)
    {
        MaterialAsset effective = asset;

        if (instance.shaderId != InvalidAssetId)
        {
            effective.shaderId = instance.shaderId;
        }

        AikoUPtr<Material> material = resolve(effective, assets, renderer);

        if (instance.runtimeDiffuseTexture != nullptr)
        {
            material->m_runtimeDiffuseTexture = instance.runtimeDiffuseTexture;
            material->m_diffuseTextureId = InvalidAssetId;
        }

        return material;
    }

}
