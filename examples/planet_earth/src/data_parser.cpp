#include "data_parser.h"

#include "math/math.h"
#include "models/mesh_factory.h"

#include <logger/logger.h>

#include <cmath>

namespace pe
{

    aiko::MeshAsset DataGenerator::generateMeshAsset(const EarthHeightmap& heightmap)
    {

        AIKO_ASSERT(heightmap.width  > 2 , "Invalid heightmap data. Width too small");
        AIKO_ASSERT(heightmap.height > 2 , "Invalid heightmap data. Height too small");

        AIKO_ASSERT(heightmap.latitudes.size() == heightmap.height, "Invalid heightmap data. Latitudes size don't match map dimensions");
        AIKO_ASSERT(heightmap.longitudes.size() == heightmap.width, "Invalid heightmap data. Longitudes size don't match map dimensions");

        AIKO_ASSERT(heightmap.elevations.size() == heightmap.width * heightmap.height, "Invalid heightmap data. Elevations size don't match map dimensions");

        constexpr size_t maxColumns = 360;
        constexpr size_t maxRows = 180;

        const size_t stride = std::max({size_t{1}, (heightmap.width + maxColumns - 1) / maxColumns, (heightmap.height + maxRows - 1) / maxRows});

        const size_t columns = (heightmap.width + stride - 1) / stride;
        const size_t rows = (heightmap.height + stride - 1) / stride;

        const size_t rowStride = columns;
        const size_t vertexCount = rowStride * rows;

        AIKO_ASSERT(vertexCount <= static_cast<size_t>(std::numeric_limits<uint16_t>::max()) + 1, "Generated mesh exceeds uint16 index limits");

        const MapElevationData data = calculateElevationData(heightmap);

        aiko::MeshAsset asset;

        asset.m_vertices.reserve(vertexCount);
        asset.m_textCoord.reserve(vertexCount);
        asset.m_colors.reserve(vertexCount);
        asset.m_indices.reserve((rows - 1) * (columns - 1) * 6);

        for (size_t y = 0 ; y < rows; ++y)
        {
            for (size_t x = 0 ; x < columns; ++x)
            {

                const size_t sourceX = x * stride;
                const size_t sourceY = y * stride;

                const size_t elevationIdx = sourceY * heightmap.width + sourceX;
                const float elevation = heightmap.elevations[elevationIdx];

                const float finalElevation = aiko::math::map(elevation, data.minElevation, data.maxElevation, 0.0f, 1.0f);
                const float amplitude = 10.0f;

                asset.m_vertices.push_back(aiko::vec3(x * stride, finalElevation * amplitude, y * stride));

                const float u = static_cast<float>(x) / static_cast<float>(columns);
                const float v = static_cast<float>(y) / static_cast<float>(rows - 1);
                asset.m_textCoord.push_back({u, v});

                const aiko::Color vertexColor = aiko::Color::lerp(aiko::BLUE, aiko::GREEN, finalElevation);

                asset.m_colors.push_back(vertexColor);
            }
        }

        // Determine grid orientation for Aiko's normal convention.
        const bool latitudeAscending = heightmap.latitudes[1] > heightmap.latitudes[0];
        const bool longitudeAscending = heightmap.longitudes[1] > heightmap.longitudes[0];

        const bool sameDirection = latitudeAscending == longitudeAscending;

        // Connect adjacent grid cells with two triangles each.
        for (size_t y = 0; y + 1 < rows; ++y)
        {
            for (size_t x = 0; x + 1 < columns; ++x)
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

        aiko::mesh::factory::recalculateNormals(asset);

        aiko::logger::Log::infoFmt("Generated Earth mesh: {} vertices, {} triangles", asset.m_vertices.size(), asset.m_indices.size() / 3);

        return asset;

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
