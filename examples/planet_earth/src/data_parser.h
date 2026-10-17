#pragma once

#include "assets/types/mesh_asset.h"
#include "planet_earth_types.h"

#include <limits>

namespace pe
{

    class DataGenerator
    {
    public:

        std::vector<GeneratedTerrainChunk> generateWorldMeshAssets(const EarthHeightmap&);

    private:

        struct MapElevationData
        {
            float minElevation = std::numeric_limits<float>::max();
            float maxElevation = std::numeric_limits<float>::lowest();
        };

        static constexpr float WorldRadius = 100.0f;
        static constexpr float EarthRadiusMeters = 6'371'000.0f;
        static constexpr float elevationScale = WorldRadius / EarthRadiusMeters;

        static constexpr float TerrainExaggeration = 50.0f;

        MapElevationData calculateElevationData(const EarthHeightmap&);

        std::vector<TerrainChunkRegion> generateChunkRegions(const EarthHeightmap&);
        aiko::MeshAsset generateMeshAsset(const MapElevationData&, const TerrainChunkRegion&, const EarthHeightmap&);

    };

}
