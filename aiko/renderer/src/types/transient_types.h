#pragma once

#include <cstdint>
#include <vector>

#include <aiko_types.h>
#include <math/math_vector.h>

#include "types/color.h"

namespace aiko
{
    class Material;

    enum class TransientTopology
    {
        Points,
        Lines,
        Triangles
    };

    struct TransientVertex
    {
        vec3 position = vec3(0.0f);
        vec2 uv = vec2(0.0f);
        vec3 normal = vec3(0.0f);
        Color color = WHITE;
    };

    struct TransientGeometry
    {
        TransientTopology topology = TransientTopology::Triangles;

        std::vector<TransientVertex> vertices;
        std::vector<uint16_t> indices;
    };

    struct TransientDrawDesc
    {
        mat4 mtx = mat4(1.0f);
        const Material* material = nullptr;
        const TransientGeometry* geometry = nullptr;
    };
}
