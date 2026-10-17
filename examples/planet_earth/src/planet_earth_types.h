#pragma once

#include <core/transform.h>
#include <models/mesh.h>
#include <assets/types/mesh_asset.h>

#include <vector>

namespace pe
{

    constexpr const size_t CHUNK_SIZE = 128;

    constexpr float WorldRadius = 100.0f;
    constexpr float EarthRadiusMeters = 6'371'000.0f;
    constexpr float elevationScale = WorldRadius / EarthRadiusMeters;

    constexpr float TerrainExaggeration = 50.0f;

    struct EarthHeightmap
    {
        size_t width = 0;
        size_t height = 0;

        std::vector<double> latitudes;
        std::vector<double> longitudes;
        std::vector<float> elevations;
    };

    struct TerrainChunkRegion
    {
        size_t startX = 0;
        size_t startY = 0;
        size_t cellWidth = 0;
        size_t cellHeight = 0;
    };

    struct GeneratedTerrainChunk
    {
        TerrainChunkRegion region;
        aiko::MeshAsset asset;
    };

    struct TerrainChunk
    {
        aiko::Transform transform;
        aiko::Mesh mesh;

        explicit TerrainChunk(const GeneratedTerrainChunk& generated)
            : mesh(generated.asset)
        {
            transform.position = {};
        }

        ~TerrainChunk()
        {
            transform = {};
            mesh.unload();
        }

    };

}

