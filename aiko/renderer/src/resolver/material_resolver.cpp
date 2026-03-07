#include "material_resolver.h"

#include "aiko_renderer.h"

namespace aiko
{

    Material MaterialResolver::resolve(const MaterialAsset& materialAsset, AikoRenderer& renderer)
    {
        Material material{};

        material.m_useVertexColor = materialAsset.useVertexColor;
        material.m_lit = materialAsset.lit;
        material.m_baseColor = materialAsset.baseColor;

        if (materialAsset.shaderId != InvalidAssetId)
        {
            // TODO: resolve ShaderAsset -> runtime Shader once shader asset loading is implemented.
            material.m_shader.load("model");
        }
        else
        {
            material.m_shader.load("model");
        }

        if (materialAsset.diffuseTextureId != InvalidAssetId)
        {
            material.m_diffuseTexture = renderer.resources().getTexture(materialAsset.diffuseTextureId);
        }

        return material;
    }

    Material MaterialResolver::resolve(const MaterialAsset& asset, const MaterialInstance& instance, AikoRenderer& renderer)
    {
        MaterialAsset effective = asset;

        if (instance.shaderId != InvalidAssetId)
        {
            effective.shaderId = instance.shaderId;
        }

        return resolve(effective, renderer);
    }

}
