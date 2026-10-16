#pragma once

#include <math/math_vector.h>

namespace aiko::editor
{

    struct TransformState
    {
        vec3 position = {};
        vec3 rotation = {};
        vec3 scale = {1.0f, 1.0f, 1.0f};

        bool operator==(const TransformState& other) const
        {
            return
                position.x == other.position.x &&
                position.y == other.position.y &&
                position.z == other.position.z &&

                rotation.x == other.rotation.x &&
                rotation.y == other.rotation.y &&
                rotation.z == other.rotation.z &&

                scale.x == other.scale.x &&
                scale.y == other.scale.y &&
                scale.z == other.scale.z;
        }

    };

}