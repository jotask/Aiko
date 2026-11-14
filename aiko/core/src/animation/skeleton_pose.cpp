#include "skeleton_pose.h"

namespace aiko
{
    bool SkeletonPoseUtils::validateSkeleton(const SkeletonAsset& skeleton)
    {
        if (skeleton.joints.empty() == true)
        {
            return false;
        }
        for (size_t i = 0; i < skeleton.joints.size(); ++i)
        {
            const SkeletonJoint& join = skeleton.joints[i];
            if (join.parentIndex == -1)
            {
                continue;
            }
            if (join.parentIndex < -1 || join.parentIndex >= i)
            {
                return false;
            }
        }
        return true;
    }

    void SkeletonPoseUtils::resetToBindPose(const SkeletonAsset& skeleton, SkeletonPose& pose)
    {
        pose.localTransforms.resize(skeleton.joints.size());
        for (size_t i = 0; i < skeleton.joints.size(); ++i)
        {
            pose.localTransforms[i] = skeleton.joints[i].localBindTransform;
        }
    }

    void SkeletonPoseUtils::calculateGlobalTransforms(const SkeletonAsset& skeleton, const SkeletonPose& pose, std::span<mat4> outGlobalTransforms)
    {
        AIKO_ASSERT(validateSkeleton(skeleton) == true, "Invalid skeleton");
        AIKO_ASSERT(pose.localTransforms.size() == skeleton.joints.size(), "Transforms and joins don't match");
        AIKO_ASSERT(outGlobalTransforms.size() == skeleton.joints.size(), "Global transform are not the same as skeleton joins");

        for (size_t i = 0; i < skeleton.joints.size(); ++i)
        {
            const SkeletonJoint& join = skeleton.joints[i];
            const aiko::mat4& local = pose.localTransforms[i];
            if (join.parentIndex == -1)
            {
                outGlobalTransforms[i] = local;
            }
            else
            {
                outGlobalTransforms[i] = outGlobalTransforms[join.parentIndex] * local;
            }
        }
    }

    void SkeletonPoseUtils::calculateSkinningMatrices(const SkeletonAsset& skeleton, std::span<const mat4> globalTransforms, std::span<mat4> outSkinningMatrices)
    {
        AIKO_ASSERT(validateSkeleton(skeleton) == true, "Invalid skeleton");
        AIKO_ASSERT(globalTransforms.size() == skeleton.joints.size(), "Transforms and joins don't match");
        AIKO_ASSERT(outSkinningMatrices.size() == skeleton.joints.size(), "Global transform are not the same as skeleton joins");

        for (size_t i = 0; i < skeleton.joints.size(); ++i)
        {
            const SkeletonJoint& join = skeleton.joints[i];
            outSkinningMatrices[i] = globalTransforms[i] * join.inverseGlobalBindTransform;
        }
    }
}
