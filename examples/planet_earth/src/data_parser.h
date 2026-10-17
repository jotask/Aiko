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

        MapElevationData calculateElevationData(const EarthHeightmap&);

        std::vector<TerrainChunkRegion> generateChunkRegions(const EarthHeightmap&);
        aiko::MeshAsset generateMeshAsset(const TerrainChunkRegion&, const EarthHeightmap&);

    };

}
