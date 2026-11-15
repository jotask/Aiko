#include "models/model.h"

#include "models/mesh.h"
#include "assets/types/mesh_asset.h"

#include <animation/skeleton_pose.h>
#include <math/math_transform.h>

#include <algorithm>
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

    bool Model::setJointRotation(std::string_view jointName, float angleRadians, const vec3& axis)
    {
        if (!m_state || !m_state->skeleton)
        {
            return false;
        }

        const SkeletonAsset& skeleton = *m_state->skeleton;

        const auto it = std::find_if(
            skeleton.joints.begin(),
            skeleton.joints.end(),
            [jointName](const SkeletonJoint& joint)
            {
                if (joint.name == jointName)
                {
                    return true;
                }

                const size_t prefixSize =
                    joint.name.size() > jointName.size()
                        ? joint.name.size() - jointName.size()
                        : 0;

                return prefixSize > 0 &&
                       joint.name[prefixSize - 1] == ':' &&
                       joint.name.compare(prefixSize, jointName.size(), jointName) == 0;
            });

        if (it == skeleton.joints.end())
        {
            return false;
        }

        if (m_pose.localTransforms.size() != skeleton.joints.size())
        {
            SkeletonPoseUtils::resetToBindPose(skeleton, m_pose);
        }

        const size_t jointIndex = static_cast<size_t>(it - skeleton.joints.begin());

        m_pose.localTransforms[jointIndex] = math::rotate(skeleton.joints[jointIndex].localBindTransform, angleRadians, axis);

        std::vector<mat4> globals(skeleton.joints.size());

        SkeletonPoseUtils::calculateGlobalTransforms(skeleton, m_pose, globals);
        m_instanceSkinMatrices.resize(skeleton.joints.size());

        SkeletonPoseUtils::calculateSkinningMatrices(skeleton, globals, m_instanceSkinMatrices);

        return true;
    }


    void Model::unload()
    {
        m_pose.localTransforms.clear();
        m_instanceSkinMatrices.clear();
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

        m_pose.localTransforms.clear();
        m_instanceSkinMatrices.clear();

        SkeletonPose pose{};
        SkeletonPoseUtils::resetToBindPose(skeleton, pose);

        std::vector<mat4> globalTransforms(jointCount);

        SkeletonPoseUtils::calculateGlobalTransforms(skeleton, pose, globalTransforms);
        SkeletonPoseUtils::calculateSkinningMatrices(skeleton, globalTransforms, m_state->bindSkinMatrices);
    }

    std::span<const mat4> Model::bindSkinMatrices() const
    {
        if (!m_instanceSkinMatrices.empty())
        {
            return m_instanceSkinMatrices;
        }
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
