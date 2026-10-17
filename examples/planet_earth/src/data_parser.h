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

        aiko::Color getTerrainColor(float elevation);

        MapElevationData calculateElevationData(const EarthHeightmap&);

        std::vector<TerrainChunkRegion> generateChunkRegions(const EarthHeightmap&);
        aiko::MeshAsset generateMeshAsset(const MapElevationData&, const TerrainChunkRegion&, const EarthHeightmap&);

        aiko::vec3 getSphericalPosition(const EarthHeightmap&, size_t, size_t);

    };

}
