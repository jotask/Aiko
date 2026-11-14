#pragma once

#include "aiko_types.h"
#include "math/math_vector.h"

namespace aiko
{

    struct SkeletonJoint
    {
        std::string name;
        int32_t parentIndex = -1;
        mat4 localBindTransform = {1.0f};
        mat4 inverseGlobalBindTransform = {1.0f};
    };

    struct SkeletonAsset
    {
        std::vector<SkeletonJoint> joints;
    };

}
