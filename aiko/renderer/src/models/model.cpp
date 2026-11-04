#include "models/model.h"

#include "models/mesh.h"
#include "assets/types/mesh_asset.h"

#include <utility>

namespace aiko
{
    void Model::addSubMesh(std::shared_ptr<Mesh> mesh, Material material)
    {
        AIKO_ASSERT(mesh != nullptr, "Model submesh requires a valid Mesh");

        if (!m_state)
        {
            m_state = std::make_shared<State>();
        }

        m_state->subMeshes.push_back(SubMesh{
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
        m_state.reset();
    }

    bool Model::isValid() const
    {
        const auto& subMeshes = getSubMeshes();

        if (subMeshes.empty())
        {
            return false;
        }

        for (const SubMesh& submesh : subMeshes)
        {
            if (!submesh.mesh || !submesh.mesh->isValid())
            {
                return false;
            }
        }

        return true;
    }

    const std::vector<Model::SubMesh>& Model::getSubMeshes() const
    {
        static const std::vector<SubMesh> empty;

        return m_state ? m_state->subMeshes : empty;
    }

    Model Model::share() const
    {
        Model model;
        model.m_state = m_state;
        return model;
    }

    void Model::replaceContents(Model&& replacement)
    {
        if (!m_state)
        {
            m_state = std::make_shared<State>();
        }

        AIKO_ASSERT(replacement.m_state != nullptr, "Replacement model has no runtime state");

        m_state->subMeshes.swap(replacement.m_state->subMeshes);
    }

    void Model::clearContents()
    {
        if (m_state)
        {
            m_state->subMeshes.clear();
        }
    }
}
