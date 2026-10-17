#include "data_parser.h"

#include "math/math.h"
#include "models/mesh_factory.h"

#include <logger/logger.h>

#include <cmath>

namespace pe
{


    std::vector<GeneratedTerrainChunk> DataGenerator::generateWorldMeshAssets(const EarthHeightmap& heightmap)
    {

        AIKO_ASSERT(heightmap.latitudes.size() == heightmap.height, "Invalid heightmap data. Latitudes size don't match map dimensions");
        AIKO_ASSERT(heightmap.longitudes.size() == heightmap.width, "Invalid heightmap data. Longitudes size don't match map dimensions");
        AIKO_ASSERT(heightmap.elevations.size() == heightmap.width * heightmap.height, "Invalid heightmap data. Elevations size don't match map dimensions");

        const MapElevationData data = calculateElevationData(heightmap);
        const std::vector<TerrainChunkRegion> regions = generateChunkRegions(heightmap);
        std::vector<GeneratedTerrainChunk> assets;
        for (const TerrainChunkRegion& region : regions)
        {
            assets.push_back(GeneratedTerrainChunk{
                .region = region,
                .asset = generateMeshAsset(data, region, heightmap),
            });
        }
        return assets;
    }

    std::vector<TerrainChunkRegion> DataGenerator::generateChunkRegions(const EarthHeightmap& heightmap)
    {

        AIKO_ASSERT(heightmap.width  > 2 , "Invalid heightmap data. Width too small");
        AIKO_ASSERT(heightmap.height > 2 , "Invalid heightmap data. Height too small");
        AIKO_ASSERT(CHUNK_SIZE > 0, "Invalid chunk size");

        const size_t totalCellsWidth  = heightmap.width - 1;
        const size_t totalCellsHeight = heightmap.height - 1;

        const size_t chunksX = (totalCellsWidth - 1) / CHUNK_SIZE + 1;
        const size_t chunksY = (totalCellsHeight - 1) / CHUNK_SIZE + 1;

        std::vector<TerrainChunkRegion> regions;
        regions.reserve(chunksX * chunksY);

        for (size_t y = 0 ; y < totalCellsHeight; y += CHUNK_SIZE)
        {
            for (size_t x = 0 ; x < totalCellsWidth; x += CHUNK_SIZE)
            {
                const size_t cellWidth = std::min(CHUNK_SIZE, totalCellsWidth - x);
                const size_t cellHeight = std::min(CHUNK_SIZE, totalCellsHeight - y);
                regions.push_back(TerrainChunkRegion{
                    .startX = x,
                    .startY = y,
                    .cellWidth = cellWidth,
                    .cellHeight = cellHeight,
                });
            }
            regions.push_back(TerrainChunkRegion{
                .startX = heightmap.width - 1,
                .startY = y,
                .cellWidth = 1,
                .cellHeight = std::min(CHUNK_SIZE, totalCellsHeight - y),
            });
        }


        return regions;
    }

    aiko::MeshAsset DataGenerator::generateMeshAsset(const MapElevationData& data, const TerrainChunkRegion& region, const EarthHeightmap& heightmap)
    {

        const size_t vertexCount = (region.cellWidth + 1) * (region.cellHeight + 1);

        AIKO_ASSERT(vertexCount <= static_cast<size_t>(std::numeric_limits<uint16_t>::max()) + 1, "Generated mesh exceeds uint16 index limits");

        aiko::MeshAsset asset;

        asset.m_vertices.reserve(vertexCount);
        asset.m_textCoord.reserve(vertexCount);
        asset.m_colors.reserve(vertexCount);
        asset.m_normals.reserve(vertexCount);
        // asset.m_indices.reserve((rows - 1) * (columns - 1) * 6);

        constexpr float amplitude = 10.0f;
        auto getElevation = [&](size_t sourceX, size_t sourceY) -> float
        {
            const size_t elevationIdx = sourceY * heightmap.width + sourceX;
            const float elevation = heightmap.elevations[elevationIdx];
            const float finalElevation = aiko::math::map(elevation, data.minElevation, data.maxElevation, 0.0f, 1.0f);
            return finalElevation;
        };

        for (size_t y = 0; y <= region.cellHeight; ++y)
        {
            for (size_t x = 0; x <= region.cellWidth; ++x)
            {

                const size_t sourceX = (region.startX + x) % heightmap.width;
                const size_t sourceY = region.startY + y;

                const float finalElevation = getElevation(sourceX, sourceY);

                const aiko::vec3 position = getSphericalPosition(heightmap, sourceX, sourceY);

                asset.m_vertices.push_back(position);

                const float u = static_cast<float>(x) / static_cast<float>(region.cellWidth);
                const float v = static_cast<float>(y) / static_cast<float>(region.cellHeight);
                asset.m_textCoord.push_back({u, v});

                const aiko::Color vertexColor = aiko::Color::lerp(aiko::BLUE, aiko::GREEN, finalElevation);

                asset.m_colors.push_back(vertexColor);

                const size_t xLeft = sourceX > 0 ? sourceX - 1 : heightmap.width - 1;
                const size_t xRight = (sourceX + 1) % heightmap.width;
                const size_t yUp = sourceY > 0 ? sourceY - 1 : sourceY;
                const size_t yDown = std::min(sourceY + 1, heightmap.height - 1);

                const aiko::vec3 leftPosition = getSphericalPosition(heightmap, xLeft, sourceY);
                const aiko::vec3 rightPosition = getSphericalPosition(heightmap, xRight, sourceY);
                const aiko::vec3 upPosition = getSphericalPosition(heightmap, sourceX, yUp);
                const aiko::vec3 downPosition = getSphericalPosition(heightmap, sourceX, yDown);

                const aiko::vec3 longitudeTangent = rightPosition - leftPosition;
                const aiko::vec3 latitudeTangent = downPosition - upPosition;

                const aiko::vec3 surfaceNormal = aiko::math::cross(longitudeTangent, latitudeTangent);
                const aiko::vec3 normal = aiko::math::lengthSquared(surfaceNormal) > 1e-12f ? aiko::math::normalize(surfaceNormal) : aiko::math::normalize(position);

                asset.m_normals.push_back(normal);

            }
        }

        // Determine grid orientation for Aiko's normal convention.
        const bool latitudeAscending = heightmap.latitudes[1] > heightmap.latitudes[0];
        const bool longitudeAscending = heightmap.longitudes[1] > heightmap.longitudes[0];

        const bool sameDirection = latitudeAscending == longitudeAscending;

        // Connect adjacent grid cells with two triangles each.
        const size_t rowStride = region.cellWidth + 1;
        for (size_t y = 0; y < region.cellHeight; ++y)
        {
            for (size_t x = 0; x < region.cellWidth; ++x)
            {
                const auto a = static_cast<uint16_t>(y * rowStride + x);
                const auto b = static_cast<uint16_t>(y * rowStride + x + 1);
                const auto c = static_cast<uint16_t>((y + 1) * rowStride + x);
                const auto d = static_cast<uint16_t>((y + 1) * rowStride + x + 1);

                if (sameDirection)
                {
                    asset.m_indices.insert(asset.m_indices.end(), {a, b, c, b, d, c});
                }
                else
                {
                    asset.m_indices.insert(asset.m_indices.end(), {a, c, b, b, c, d});
                }
            }
        }

        aiko::logger::Log::infoFmt("Generated Earth mesh: {} vertices, {} triangles", asset.m_vertices.size(), asset.m_indices.size() / 3);

        return asset;

    }

    aiko::vec3 DataGenerator::getSphericalPosition(const EarthHeightmap& heightmap, size_t sourceX, size_t sourceY)
    {
        const float radianLatitude  = aiko::math::radians( heightmap.latitudes [sourceY] );
        const float radianLongitude = aiko::math::radians( heightmap.longitudes[sourceX] );

        const float elevationMeters = heightmap.elevations[sourceY * heightmap.width + sourceX];
        const float radius = WorldRadius + elevationMeters * elevationScale * TerrainExaggeration;

        const float sphericalX = radius * aiko::math::cos(radianLatitude) * aiko::math::cos(radianLongitude);
        const float sphericalY = radius * aiko::math::sin(radianLatitude);
        const float sphericalZ = -radius * aiko::math::cos(radianLatitude) * aiko::math::sin(radianLongitude);

        return {sphericalX, sphericalY, sphericalZ};
    }

    DataGenerator::MapElevationData DataGenerator::calculateElevationData(const EarthHeightmap& data)
    {
        MapElevationData result;
        for (const float elevation : data.elevations)
        {
            if (std::isfinite(elevation) == false)
            {
                continue;
            }
            result.minElevation = std::min(result.minElevation, elevation);
            result.maxElevation = std::max(result.maxElevation, elevation);
        }

        aiko::logger::Log::infoFmt("Elevation data => min:{}, max:{}", result.minElevation, result.maxElevation);

        return result;
    }

}
