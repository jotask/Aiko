#include "models/model.h"

#include "models/mesh.h"
#include "assets/types/mesh_asset.h"

#include <animation/skeleton_pose.h>
#include <animation/animation_pose.h>
#include <math/math_transform.h>

#include <cmath>
#include <algorithm>
#include <utility>

namespace aiko
{
    void Model::addSubMesh(std::shared_ptr<Mesh> mesh, Material material, bool skinned, std::vector<std::string> morphTargetNames)
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
            .morphTargetNames = std::move(morphTargetNames),
        });
    }

    void Model::addSubMesh(const MeshAsset& asset, Material material)
    {
        auto mesh = std::make_shared<Mesh>();
        mesh->upload(asset);

        std::vector<std::string> names;
        names.reserve(asset.m_morphTargets.size());

        for (const MorphTargetAsset& target : asset.m_morphTargets)
        {
            names.push_back(target.name);
        }

        addSubMesh(std::move(mesh), std::move(material), !asset.m_jointIndices.empty(), std::move(names));
    }

    bool Model::setMorphWeight(std::string_view name, float weight)
    {
        if (name.empty() || !std::isfinite(weight))
        {
            return false;
        }

        bool found = false;

        for (const SubMesh& submesh : getSubMeshes())
        {
            const auto it = std::find_if(
                submesh.morphTargetNames.begin(),
                submesh.morphTargetNames.end(),
                [name](const std::string& targetName)
                {
                    return targetName == name;
                });

            if (it != submesh.morphTargetNames.end())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            return false;
        }

        if (weight == 0.0f)
        {
            m_morphWeights.erase(std::string(name));
        }
        else
        {
            m_morphWeights.insert_or_assign(std::string(name), weight);
        }

        return true;
    }

    float Model::morphWeight(std::string_view name) const
    {
        const auto it = m_morphWeights.find(std::string(name));

        return it != m_morphWeights.end() ? it->second : 0.0f;
    }

    std::vector<float> Model::morphWeightsForSubMesh(size_t subMeshIndex) const
    {
        const auto submeshes = getSubMeshes();

        if (subMeshIndex >= submeshes.size())
        {
            return {};
        }

        const SubMesh& submesh = submeshes[subMeshIndex];

        std::vector<float> weights;
        weights.reserve(submesh.morphTargetNames.size());

        for (const std::string& name : submesh.morphTargetNames)
        {
            weights.push_back(morphWeight(name));
        }

        return weights;
    }

    bool Model::setJointRotation(std::string_view jointName, float angleRadians, const vec3& axis)
    {
        const auto index = findJointIndex(jointName);

        if (!index)
        {
            return false;
        }

        SkeletonPose pose = m_pose.localTransforms.size() == joints().size() ? m_pose : createBindPose();

        pose.localTransforms[*index] = math::rotate(joints()[*index].localBindTransform, angleRadians, axis);

        return setPose(pose);
    }

    std::span<const SkeletonJoint> Model::joints() const
    {
        if (!m_state || !m_state->skeleton)
        {
            return {};
        }

        return std::span<const SkeletonJoint>(m_state->skeleton->joints);
    }

    std::optional<size_t> Model::findJointIndex(std::string_view jointName) const
    {
        if (jointName.empty())
        {
            return std::nullopt;
        }

        const auto skeletonJoints = joints();

        // Exact names take priority.
        for (size_t i = 0; i < skeletonJoints.size(); ++i)
        {
            if (skeletonJoints[i].name == jointName)
            {
                return i;
            }
        }

        // Also accept Mixamo-style names without namespace.
        std::optional<size_t> result;

        for (size_t i = 0; i < skeletonJoints.size(); ++i)
        {
            const std::string_view name = skeletonJoints[i].name;

            if (name.size() <= jointName.size())
            {
                continue;
            }

            const size_t start = name.size() - jointName.size();

            if (name[start - 1] != ':' || name.substr(start) != jointName)
            {
                continue;
            }

            // Reject ambiguous short names.
            if (result)
            {
                return std::nullopt;
            }

            result = i;
        }

        return result;
    }

    SkeletonPose Model::createBindPose() const
    {
        SkeletonPose pose{};

        if (m_state && m_state->skeleton)
        {
            SkeletonPoseUtils::resetToBindPose(*m_state->skeleton, pose);
        }

        return pose;
    }

    bool Model::setPose(const SkeletonPose& pose)
    {
        if (!m_state || !m_state->skeleton)
        {
            return false;
        }

        const SkeletonAsset& skeleton = *m_state->skeleton;

        if (pose.localTransforms.size() != skeleton.joints.size())
        {
            return false;
        }

        // Tracking data can contain invalid measurements.
        for (const mat4& local : pose.localTransforms)
        {
            for (int i = 0; i < 16; ++i)
            {
                if (!std::isfinite(local.data()[i]))
                {
                    return false;
                }
            }
        }

        // External poses take control over clip playback.
        m_animationPlaying = false;
        m_activeAnimationIndex.reset();
        m_animationTime = 0.0;

        // Also supports setPose(m_pose).
        if (&pose != &m_pose)
        {
            m_pose = pose;
        }

        std::vector<mat4> globalTransforms(skeleton.joints.size());

        SkeletonPoseUtils::calculateGlobalTransforms(skeleton, m_pose, globalTransforms);

        m_instanceSkinMatrices.resize(skeleton.joints.size());

        SkeletonPoseUtils::calculateSkinningMatrices(skeleton, globalTransforms, m_instanceSkinMatrices);

        return true;
    }

    bool Model::setJointLocalTransform(size_t jointIndex, const mat4& localTransform)
    {
        const auto skeletonJoints = joints();

        if (jointIndex >= skeletonJoints.size())
        {
            return false;
        }

        SkeletonPose nextPose = m_pose.localTransforms.size() == skeletonJoints.size() ? m_pose : createBindPose();

        nextPose.localTransforms[jointIndex] = localTransform;

        return setPose(nextPose);
    }

    bool Model::setJointLocalTransform(std::string_view jointName, const mat4& localTransform)
    {
        const auto index = findJointIndex(jointName);
        return index && setJointLocalTransform(*index, localTransform);
    }

    size_t Model::animationCount() const
    {
        return m_state ? m_state->animations.size() : 0;
    }

    std::string_view Model::animationName(size_t index) const
    {
        if (!m_state || index >= m_state->animations.size())
        {
            return {};
        }

        return m_state->animations[index].name;
    }

    bool Model::playAnimation(std::string_view clipName, bool loop)
    {
        if (!m_state || clipName.empty())
        {
            return false;
        }

        for (size_t i = 0; i < m_state->animations.size(); ++i)
        {
            if (m_state->animations[i].name == clipName)
            {
                return playAnimation(i, loop);
            }
        }

        return false;
    }

    bool Model::playAnimation(size_t index, bool loop)
    {
        if (!m_state || !m_state->skeleton || index >= m_state->animations.size())
        {
            return false;
        }

        m_activeAnimationIndex = index;
        m_animationTime = 0.0;
        m_animationLoop = loop;
        m_animationPlaying = true;

        SkeletonPoseUtils::resetToBindPose(*m_state->skeleton, m_pose);

        updateAnimation(0.0f);

        return true;
    }

    void Model::updateAnimation(float deltaTime)
    {
        if (!m_animationPlaying ||
            !m_activeAnimationIndex ||
            !m_state ||
            !m_state->skeleton)
        {
            return;
        }

        AIKO_ASSERT(std::isfinite(deltaTime) && deltaTime >= 0.0f, "Invalid animation delta time");

        if (*m_activeAnimationIndex >= m_state->animations.size())
        {
            stopAnimation();
            return;
        }

        const AnimationClipAsset& clip = m_state->animations[*m_activeAnimationIndex];

        const SkeletonAsset& skeleton = *m_state->skeleton;

        m_animationTime += static_cast<double>(deltaTime);

        bool finished = false;

        if (clip.durationSeconds > 0.0)
        {
            if (m_animationLoop)
            {
                m_animationTime = std::fmod(m_animationTime, clip.durationSeconds);
            }
            else if (m_animationTime >= clip.durationSeconds)
            {
                m_animationTime = clip.durationSeconds;
                finished = true;
            }
        }

        AnimationPoseUtils::sample(clip, skeleton, m_animationTime, m_pose);

        std::vector<mat4> globalTransforms(skeleton.joints.size());
        SkeletonPoseUtils::calculateGlobalTransforms(skeleton, m_pose, globalTransforms);
        m_instanceSkinMatrices.resize(skeleton.joints.size());
        SkeletonPoseUtils::calculateSkinningMatrices(skeleton, globalTransforms, m_instanceSkinMatrices);

        if (finished)
        {
            // Preserve the final pose.
            m_animationPlaying = false;
        }
    }

    void Model::stopAnimation()
    {
        m_animationPlaying = false;
        m_activeAnimationIndex.reset();
        m_animationTime = 0.0;

        m_pose.localTransforms.clear();
        m_instanceSkinMatrices.clear();
    }

    void Model::unload()
    {
        stopAnimation();
        m_morphWeights.clear();
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

        stopAnimation();

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

    void Model::setAnimations(std::vector<AnimationClipAsset> animations)
    {
        AIKO_ASSERT(m_state != nullptr, "Model has no state");
        m_state->animations = std::move(animations);
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
        m_state->animations.swap(replacement.m_state->animations);
    }

    void Model::clearContents()
    {
        if (m_state)
        {
            m_state->subMeshes.clear();
            m_state->skeleton.reset();
            m_state->bindSkinMatrices.clear();
            m_state->animations.clear();
        }
    }
}
