#pragma once

#include <variant>
#include <unordered_map>

#include <aiko_types.h>
#include <math/math.h>

namespace aiko
{

    enum class UniformType
    {
        Unknown,

        Bool,
        Int,
        UInt,
        Float,

        BVec2,
        BVec3,
        BVec4,

        IVec2,
        IVec3,
        IVec4,

        UVec2,
        UVec3,
        UVec4,

        Vec2,
        Vec3,
        Vec4,

        Mat4,
    };

    using UniformValue = std::variant<
        bool,
        int,
        u32,
        float,

        bvec2,
        bvec3,
        bvec4,

        ivec2,
        ivec3,
        ivec4,

        uvec2,
        uvec3,
        uvec4,

        vec2,
        vec3,
        vec4,

        mat4
    >;

    using UniformMap =
        std::unordered_map<string, UniformValue>;

}
