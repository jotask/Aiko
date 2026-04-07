#include "chunk_data_generator.h"

#include "world/types/chunk_types.h"
#include "world/types/world_types.h"

#include <math/math_vector.h>

#include <PerlinNoise.hpp>

namespace vw
{

    void ChunkDataGenerator::generateChunkData(const GeneratorSettings& cfg, const ChunkCoord& coord, ChunkData& data)
    {
        clearChunkData(data);

        const siv::PerlinNoise::seed_type seed = cfg.seed;
        const siv::PerlinNoise perlin {seed};

        const auto baseHeight = CHUNK_SIZE.y / 2;

        for (std::size_t z = 0 ; z < CHUNK_SIZE.z; ++z)
        {
            for (std::size_t x = 0 ; x < CHUNK_SIZE.x; ++x)
            {

                const aiko::ivec2 voxelWorldPos = ( coord * aiko::ivec2(CHUNK_SIZE.x, CHUNK_SIZE.z ) ) + aiko::ivec2(x, z);
                const aiko::vec2 noiseSampleCoord = aiko::vec2(voxelWorldPos.x,voxelWorldPos.y) * cfg.noiseScale;
                const double noise = perlin.octave2D(noiseSampleCoord.x, noiseSampleCoord.y, cfg.octaves, cfg.persistance);

                const auto height = baseHeight + noise * cfg.amplitude;

                for (std::size_t y = 0 ; y < CHUNK_SIZE.y; ++y)
                {
                    VoxelType type = VoxelType::AIR;
                    if ( y < height )
                    {
                        type = VoxelType::DIRT;
                    }
                    data[z][y][x] = type;
                }
            }
        }
    }

    void ChunkDataGenerator::clearChunkData(ChunkData& data)
    {
        for (auto& z : data)
        {
            for (auto& y : z)
            {
                for (auto& x : y)
                {
                    x = VoxelType::AIR;
                }
            }
        }
    }

}

