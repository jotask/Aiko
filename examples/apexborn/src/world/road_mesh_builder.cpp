#include "road_mesh_builder.h"

namespace ab
{

    aiko::MeshAsset RoadMeshBuilder::build(const Spline& spline, const RoadProfile& profile, const RoadMeshBuilderDesc& desc)
    {

        AIKO_ASSERT(spline.getSamplePoints().empty() == false, "We can't generate mesh from empty spline");
        AIKO_ASSERT(profile.points.empty() == false, "We can't generate mesh from empty profiler");
        AIKO_ASSERT(profile.closed ? profile.points.size() >= 3 : profile.points.size() >= 2, "Invalid road profile point count");
        AIKO_ASSERT(desc.roadWidth > 0.0f, "Road width musth be positive");

        aiko::MeshAsset asset;

        const auto& centerLine = spline.getSamplePoints();

        const size_t vertexCount = centerLine.size() * profile.points.size();
        asset.m_vertices.reserve(vertexCount);

        // Vertices
        for (size_t i = 0 ; i < centerLine.size() ; ++i)
        {
            const auto tangent = spline.getSampleTangent(i);
            const auto right = aiko::math::normalize(aiko::math::cross(tangent, desc.worldUp));
            const auto up = aiko::math::normalize(aiko::math::cross(right, tangent));

            for (size_t j = 0 ; j < profile.points.size(); ++j)
            {
                const auto position = centerLine[i];
                const auto profilePoint = profile.points[j];
                const auto worldPosition = position + right * ( profilePoint.x * desc.roadWidth ) + up * profilePoint.y;
                asset.m_vertices.push_back(worldPosition);
            }
        }

        // Indices

        const size_t ringCount = centerLine.size();
        const size_t profileCount = profile.points.size();
        const size_t edgeCount = profile.closed ? profileCount : profileCount - 1;

        asset.m_indices.reserve(ringCount * edgeCount * 6);

        for (size_t i = 0 ; i < ringCount ; ++i)
        {
            const auto currentRing = i;
            const auto nextRing = (i + 1) % ringCount;

            for (size_t j = 0 ; j < edgeCount ; ++j)
            {
                const auto nextProfile = (j + 1) % profileCount;

                const size_t a = currentRing * profileCount + j;
                const size_t b = currentRing * profileCount + nextProfile;
                const size_t c = nextRing * profileCount + j;
                const size_t d = nextRing * profileCount + nextProfile;

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

        AIKO_ASSERT(asset.m_vertices.size() == ringCount * profileCount, "Incorrect road vertex count");
        AIKO_ASSERT(asset.m_indices.size() == ringCount * edgeCount * 6, "Incorrect road index count");

        return asset;
    }
}
