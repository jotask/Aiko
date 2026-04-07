#pragma once

#include <array>

#include "math/math_vector.h"

namespace vw
{

    constexpr std::size_t VOXEL_SIZE = 1;
    constexpr aiko::ivec3 CHUNK_SIZE = {16, 64, 16};

    using ChunkCoord = aiko::ivec2;

    enum class VoxelType
    {
        AIR,
        DIRT,
    };

    using ChunkData = std::array<std::array<std::array<VoxelType, CHUNK_SIZE.x>, CHUNK_SIZE.y>, CHUNK_SIZE.z>;

}

