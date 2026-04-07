#pragma once

#include "world/types/chunk_types.h"

namespace vw
{

    class ChunkDataGenerator
    {
    public:

        struct GeneratorSettings
        {
            std::int32_t seed = 123456;
            float noiseScale = 0.01;
            std::int32_t octaves = 4;
            float persistance = 0.5f;
            float amplitude = 10.0f;
        };

        static void generateChunkData(const GeneratorSettings& cfg, const ChunkCoord& coord, ChunkData& data);
        static void clearChunkData(ChunkData& data);

        static aiko::vector<float> generatePlotTest(const GeneratorSettings& cfg, std::size_t samples );
    private:
    };

}

