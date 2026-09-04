#include "models/model.h"

namespace aiko
{

    void Model::upload(const ModelAsset& asset)
    {
        unload();
        m_subMeshes.reserve(asset.submeshes.size());

        for (const auto& submesh : asset.submeshes)
        {
            SubMesh runtimeSubMesh{};
            runtimeSubMesh.meshId = submesh.meshId;
            runtimeSubMesh.material.m_shaderId = submesh.material.shaderId;
            if (submesh.material.diffuseTextureId != InvalidAssetId)
            {
                runtimeSubMesh.material.setTexture("u_texture", submesh.material.diffuseTextureId);
            }
            runtimeSubMesh.material.m_baseColor = submesh.material.baseColor;
            runtimeSubMesh.material.m_useVertexColor = submesh.material.useVertexColor;
            runtimeSubMesh.material.m_lit = submesh.material.lit;
            m_subMeshes.push_back(std::move(runtimeSubMesh));
        }
    }

    void Model::unload()
    {
        m_subMeshes.clear();
    }

}
