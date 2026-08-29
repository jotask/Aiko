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
            runtimeSubMesh.material = submesh.material;
            m_subMeshes.push_back(runtimeSubMesh);
        }
    }

    void Model::unload()
    {
        m_subMeshes.clear();
    }

}
