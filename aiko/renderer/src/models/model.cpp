#include "models/model.h"

#include "models/mesh.h"
#include "assets/types/mesh_asset.h"

#include <utility>

namespace aiko
{
    void Model::addSubMesh(std::shared_ptr<Mesh> mesh, Material material)
    {
        AIKO_ASSERT(mesh != nullptr, "Model submesh requires a valid Mesh");

        m_subMeshes.push_back(SubMesh{
            .mesh = std::move(mesh),
            .material = std::move(material)
        });
    }

    void Model::addSubMesh(const MeshAsset& asset, Material material)
    {
        auto mesh = std::make_shared<Mesh>();
        mesh->upload(asset);

        addSubMesh(std::move(mesh), std::move(material));
    }

    void Model::unload()
    {
        m_subMeshes.clear();
    }

    bool Model::isValid() const
    {
        if (m_subMeshes.empty())
        {
            return false;
        }

        for (const SubMesh& submesh : m_subMeshes)
        {
            if (submesh.mesh == nullptr || !submesh.mesh->isValid())
            {
                return false;
            }
        }

        return true;
    }
}
