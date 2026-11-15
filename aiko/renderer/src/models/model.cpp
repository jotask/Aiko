#include "models/model.h"

#include "models/mesh.h"
#include "assets/types/mesh_asset.h"

#include <animation/skeleton_pose.h>

#include <utility>

namespace aiko
{
    void Model::addSubMesh(std::shared_ptr<Mesh> mesh, Material material, bool skinned)
    {
        AIKO_ASSERT(mesh != nullptr, "Model submesh requires a valid Mesh");

        if (!m_state)
        {
            m_state = std::make_shared<State>();
        }

        m_state->subMeshes.push_back(SubMesh{
            .mesh = std::move(mesh),
            .material = std::move(material),
            .skinned = skinned,
        });
    }

    void Model::addSubMesh(const MeshAsset& asset, Material material)
    {
        auto mesh = std::make_shared<Mesh>();

        mesh->upload(asset);

        addSubMesh(std::move(mesh), std::move(material), !asset.m_jointIndices.empty());
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

    void Model::setSkeleton(const SkeletonAsset& skeleton)
    {
        AIKO_ASSERT(m_state != nullptr, "Model has no state");

        m_state->skeleton = skeleton;

        const size_t jointCount = skeleton.joints.size();

        m_state->bindSkinMatrices.resize(jointCount);

        if (jointCount == 0)
        {
            return;
        }

        SkeletonPose pose{};
        SkeletonPoseUtils::resetToBindPose(skeleton, pose);

        std::vector<mat4> globalTransforms(jointCount);

        SkeletonPoseUtils::calculateGlobalTransforms(skeleton, pose, globalTransforms);
        SkeletonPoseUtils::calculateSkinningMatrices(skeleton, globalTransforms, m_state->bindSkinMatrices);
    }

    std::span<const mat4> Model::bindSkinMatrices() const
    {
        return m_state ? std::span<const mat4>(m_state->bindSkinMatrices) : std::span<const mat4>{};
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

        m_state->skeleton.swap(replacement.m_state->skeleton);
        m_state->bindSkinMatrices.swap(replacement.m_state->bindSkinMatrices);
    }

    void Model::clearContents()
    {
        if (m_state)
        {
            m_state->subMeshes.clear();
            m_state->skeleton.reset();
            m_state->bindSkinMatrices.clear();
        }
    }
}
