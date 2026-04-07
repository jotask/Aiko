#pragma once

#include <functional>

#include "core/utils.h"
#include "math/math_vector.h"

namespace vw
{

    constexpr size_t WORLD_SIZE = 8;

    struct ChunkKey
    {
        ChunkCoord coord = {};
        bool operator==(const ChunkKey& other) const
        {
            return coord == other.coord;
        }
    };

    struct ChunkKeyHash
    {
        std::size_t operator()(const ChunkKey& s) const noexcept
        {
            std::size_t seed = 0;
            aiko::utils::hashCombine(std::hash<int>{}(s.coord.x), seed);
            aiko::utils::hashCombine(std::hash<int>{}(s.coord.y), seed);
            return seed;
        }
    };

}

