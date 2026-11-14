#pragma once

#include "math/math_vector.h"

#include <assets/types/skeleton_asset.h>

#include <span>

namespace aiko
{
    struct SkeletonPose
    {
        std::vector<mat4> localTransforms;
    };

    class SkeletonPoseUtils final
    {
    public:
        static bool validateSkeleton(const SkeletonAsset& skeleton);
        static void resetToBindPose(const SkeletonAsset& skeleton, SkeletonPose& pose);
        static void calculateGlobalTransforms(const SkeletonAsset& skeleton, const SkeletonPose& pose, std::span<mat4> outGlobalTransforms);
        static void calculateSkinningMatrices(const SkeletonAsset& skeleton, std::span<const mat4> globalTransforms, std::span<mat4> outSkinningMatrices);
    };

}
