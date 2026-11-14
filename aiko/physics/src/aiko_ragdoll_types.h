#pragma once

#include "aiko_physics_types.h"
#include "aiko_shape_desc.h"

#include <core/transform.h>

#include <limits>
#include <vector>
#include <string>

namespace aiko::physics
{

    using RagdollId = u32;
    constexpr RagdollId InvalidRagdollId = std::numeric_limits<RagdollId>::max();

    struct RagdollPartDesc
    {
        std::string name;
        int parentIndex = -1;
        Transform transform;
        ShapeDesc shape;
    };

    using RagdollDesc = std::vector<RagdollPartDesc>;

}