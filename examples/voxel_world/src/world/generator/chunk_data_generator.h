#pragma once

#include "world/types/chunk_types.h"

namespace vw
{

    class ChunkDataGenerator
    {
    public:
        static void generateChunkData(const ChunkCoord& coord, ChunkData& data);
        static void clearChunkData(ChunkData& data);
    private:
    };

}

