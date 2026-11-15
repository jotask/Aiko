#pragma once

#include "animation/skeleton_pose.h"
#include "assets/types/animation_clip_asset.h"

namespace aiko
{
    class AnimationPoseUtils final
    {
    public:
        static void sample(const AnimationClipAsset& clip, const SkeletonAsset& skeleton, double timeSeconds, SkeletonPose& pose);
    };
}
