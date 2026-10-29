#include "road_mesh_builder.h"

#include "constant_types.h"
#include "models/mesh_factory.h"

namespace ab
{

    aiko::MeshAsset RoadMeshBuilder::build(const Spline& spline, const RoadProfile& profile, const RoadMeshBuilderDesc& desc)
    {

        const auto& centerLine = spline.getSamplePoints();

        AIKO_ASSERT(centerLine.empty() == false, "We can't generate mesh from empty spline");
        AIKO_ASSERT(centerLine.size() >= 3, "Invalid Spline");
        AIKO_ASSERT(profile.points.empty() == false, "We can't generate mesh from empty profiler");
        AIKO_ASSERT(profile.closed ? profile.points.size() >= 3 : profile.points.size() >= 2, "Invalid road profile point count");
        AIKO_ASSERT(desc.roadWidth > 0.0f, "Road width musth be positive");

        const size_t ringCount = centerLine.size();
        const size_t profileCount = profile.points.size();
        const size_t edgeCount = profile.closed ? profileCount : profileCount - 1;

        const size_t ringStride = edgeCount * 2;
        const size_t vertexCount = (ringCount  + 1) * ringStride;

        AIKO_ASSERT(vertexCount <= std::numeric_limits<uint32_t>::max(), "Road Mesh excedes uint32 index range");

        aiko::MeshAsset asset;

        asset.m_vertices.reserve(vertexCount);
        asset.m_indices.reserve(ringCount * edgeCount * 6);
        asset.m_normals.reserve(vertexCount);
        asset.m_textCoord.reserve(vertexCount);

        // Calculate cumulative distance
        std::vector<float> trackDistance(ringCount + 1, 0.0f);
        for(size_t i = 1 ; i <= ringCount; ++i)
        {
            const auto& previous = centerLine[i - 1];
            const auto& current = centerLine[i % ringCount];

            trackDistance[i] = trackDistance[i -1] + aiko::math::distance(previous, current);
        }

        const float trackLength = trackDistance.back();
        AIKO_ASSERT(trackLength > epsilon, "Invalid Track lenght");

        // Calculate cumulative profile distances
        std::vector<float> profileDistance(edgeCount + 1, 0.0f);
        for(size_t i = 0 ; i < edgeCount; ++i)
        {
            const auto& p0 = profile.points[i];
            const auto& p1 = profile.points[(i + 1) % profileCount];

            const aiko::vec2 delta = {
                (p1.x - p0.x) * desc.roadWidth,
                p1.y - p0.y
            };

            const auto edgeLength = aiko::math::length(delta);

            AIKO_ASSERT(edgeLength > epsilon, "Invalid profile edge");

            profileDistance[i + 1] = profileDistance[i] + edgeLength;

        }

        const float profileLength = profileDistance.back();
        AIKO_ASSERT(profileLength > epsilon, "Invalid Track length");

        // Integer repeat counts prevent jumps at the same UV seams
        constexpr float textureRepeatLength = 2.0f;
        const float trackRepeats = std::max(1.0f, std::round(trackLength / textureRepeatLength));
        const float profileRepeats = std::max(1.0f, std::round(profileLength / textureRepeatLength));

        // Generate vertices, normals and UVs
        for (size_t i = 0 ; i <= ringCount ; ++i)
        {

            const auto sampleIdx = i % ringCount;

            const auto position = centerLine[sampleIdx];
            const auto tangent = spline.getSampleTangent(sampleIdx);

            const auto rightVector = aiko::math::cross(tangent, desc.worldUp);
            AIKO_ASSERT(aiko::math::lengthSquared(rightVector) > epsilon, "Invalid road coordinate frame");

            const auto right = aiko::math::normalize(rightVector);
            const auto up = aiko::math::normalize(aiko::math::cross(right, tangent));

            const float u = (trackDistance[i] / trackLength) * trackRepeats;

            for (size_t j = 0 ; j < edgeCount; ++j)
            {
                const auto& p0 = profile.points[j];
                const auto& p1 = profile.points[(j + 1) % profileCount];

                const auto v0 = position + right * (p0.x * desc.roadWidth) + up * p0.y;
                const auto v1 = position + right * (p1.x * desc.roadWidth) + up * p1.y;

                const auto edgeDirection = v1 - v0;
                const auto normal = aiko::math::normalize(aiko::math::cross(edgeDirection, tangent));

                asset.m_vertices.push_back(v0);
                asset.m_vertices.push_back(v1);

                asset.m_normals.push_back(normal);
                asset.m_normals.push_back(normal);

                const float vCoord0 = (profileDistance[j + 0] / profileLength) * profileRepeats;
                const float vCoord1 = (profileDistance[j + 1] / profileLength) * profileRepeats;
                asset.m_textCoord.push_back({u, vCoord0});
                asset.m_textCoord.push_back({u, vCoord1});
            }
        }

        // Indices
        for (size_t i = 0 ; i < ringCount ; ++i)
        {
            for (size_t j = 0 ; j < edgeCount ; ++j)
            {
                const auto nextProfile = (j + 1) % profileCount;

                const size_t a = i * ringStride + j * 2;
                const size_t b = a + 1;

                const size_t c = (i + 1) * ringStride + j * 2;
                const size_t d = c + 1;

                // Triangle 1
                asset.m_indices.push_back(a);
                asset.m_indices.push_back(c);
                asset.m_indices.push_back(b);

                // Triangle 2
                asset.m_indices.push_back(b);
                asset.m_indices.push_back(c);
                asset.m_indices.push_back(d);
            }

        }

        // Validation
        AIKO_ASSERT(asset.m_vertices.size() == vertexCount, "Incorrect road vertex count");
        AIKO_ASSERT(asset.m_normals.size() == vertexCount, "Incorrect road normal count");
        AIKO_ASSERT(asset.m_textCoord.size() == vertexCount, "Incorrect road UVs count");
        AIKO_ASSERT(asset.m_indices.size() == ringCount * edgeCount * 6, "Incorrect road index count");

        return asset;
    }
}
