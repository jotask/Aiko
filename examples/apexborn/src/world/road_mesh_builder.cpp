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
        asset.m_colors.reserve(vertexCount);

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

                constexpr aiko::Color Asphalt  = {0.19f, 0.20f, 0.23f};
                constexpr aiko::Color Side     = {0.80f, 0.28f, 0.27f};
                constexpr aiko::Color Bottom   = {0.12f, 0.13f, 0.15f};

                const float upness = aiko::math::dot(normal, up);

                const aiko::Color faceColor =
                    upness > 0.5f ? Asphalt :
                    upness < -0.5f ? Bottom :
                                     Side;

                asset.m_colors.push_back(faceColor);
                asset.m_colors.push_back(faceColor);
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
        AIKO_ASSERT(asset.m_colors.size() == vertexCount, "Incorrect road vertex color count");

        return asset;
    }

    aiko::MeshAsset RoadMeshBuilder::buildEdgeMarkings(const Spline& spline, const RoadProfile& profile, const RoadMeshBuilderDesc& desc)
    {
        const auto& centerLine = spline.getSamplePoints();

        AIKO_ASSERT(centerLine.size() >= 3, "Invalid spline");
        AIKO_ASSERT(desc.roadWidth > 0.0f, "Invalid road width");
        AIKO_ASSERT(profile.points.size() >= (profile.closed ? 3u : 2u), "Invalid road profile");

        constexpr float Inset = 0.115f;
        constexpr float StripeWidth = 0.035f;
        constexpr float SurfaceOffset = 0.01f;

        // Profile X is normalized to [-0.5, +0.5].
        const std::array<float, 4> x = {
            -0.5f + Inset,
            -0.5f + Inset + StripeWidth,
             0.5f - Inset - StripeWidth,
             0.5f - Inset
        };

        std::array<float, 4> y{};

        for (size_t j = 0; j < x.size(); ++j)
        {
            const float height = getProfileTopHeight(profile, x[j]);
            y[j] = height;
        }

        const size_t ringCount = centerLine.size();
        constexpr size_t VerticesPerRing = 4;

        AIKO_ASSERT((ringCount + 1) * VerticesPerRing <= std::numeric_limits<uint32_t>::max(), "Edge marking mesh exceeds index range");

        aiko::MeshAsset asset;

        const size_t vertexCount = (ringCount + 1) * VerticesPerRing;

        asset.m_vertices.reserve(vertexCount);
        asset.m_normals.reserve(vertexCount);
        asset.m_textCoord.reserve(vertexCount);
        asset.m_colors.reserve(vertexCount);
        asset.m_indices.reserve(ringCount * 12);

        for (size_t i = 0; i <= ringCount; ++i)
        {
            const size_t sampleIdx = i % ringCount;

            const auto& position = centerLine[sampleIdx];
            const auto tangent = spline.getSampleTangent(sampleIdx);

            const auto right = aiko::math::normalize(aiko::math::cross(tangent, desc.worldUp));

            const auto up = aiko::math::normalize(aiko::math::cross(right, tangent));

            for (size_t j = 0; j < VerticesPerRing; ++j)
            {
                const auto vertex = position + right * (x[j] * desc.roadWidth) + up * (y[j] + SurfaceOffset);

                asset.m_vertices.push_back(vertex);
                asset.m_normals.push_back(up);
                asset.m_colors.push_back(aiko::WHITE);

                asset.m_textCoord.push_back({
                    static_cast<float>(i) / ringCount,
                    static_cast<float>(j % 2)
                });
            }
        }

        // Connect the four vertices of each ring into two strips.
        for (size_t i = 0; i < ringCount; ++i)
        {
            for (size_t strip = 0; strip < 2; ++strip)
            {
                const uint32_t a = static_cast<uint32_t>(i * 4 + strip * 2);

                const uint32_t b = a + 1;

                const uint32_t c = static_cast<uint32_t>((i + 1) * 4 + strip * 2);

                const uint32_t d = c + 1;

                // Same winding convention as the road surface.
                asset.m_indices.insert(asset.m_indices.end(), {a, c, b, b, c, d});
            }
        }

        return asset;
    }

    aiko::MeshAsset RoadMeshBuilder::buildCurbs(const Spline& spline, const RoadProfile& profile, const RoadMeshBuilderDesc& desc)
    {
        const auto& samples = spline.getSamplePoints();

        AIKO_ASSERT(samples.size() >= 3, "Invalid spline");
        AIKO_ASSERT(desc.roadWidth > 0.0f, "Invalid road width");
        AIKO_ASSERT(profile.points.size() >= (profile.closed ? 3u : 2u), "Invalid road profile");

        constexpr float SurfaceOffset = 0.012f;
        constexpr float DesiredStripeLength = 0.75f;

        constexpr aiko::Color Red = {0.85f, 0.16f, 0.14f};
        constexpr aiko::Color White = {0.94f, 0.93f, 0.89f};

        const std::array<float, 4> x = {
            -0.495f, -0.415f,
             0.415f,  0.495f
        };

        std::array<float, 4> y{};

        for (size_t j = 0; j < x.size(); ++j)
        {
            y[j] = getProfileTopHeight(profile, x[j]);
        }

        const size_t ringCount = samples.size();

        // Closed-loop arc lengths, including the final-to-first segment.
        std::vector<float> distances(ringCount + 1, 0.0f);

        for (size_t i = 0; i < ringCount; ++i)
        {
            const auto& a = samples[i];
            const auto& b = samples[(i + 1) % ringCount];

            const float length = aiko::math::distance(a, b);
            AIKO_ASSERT(length > epsilon, "Degenerate spline segment");

            distances[i + 1] = distances[i] + length;
        }

        const float totalLength = distances.back();

        AIKO_ASSERT(std::isfinite(totalLength) && totalLength > epsilon, "Invalid track length");

        // An even number of stripes makes the closing seam consistent.
        size_t stripeCount = std::max<size_t>(2, static_cast<size_t>( std::ceil(totalLength / DesiredStripeLength) ));

        if (stripeCount % 2 != 0)
        {
            ++stripeCount;
        }

        const float stripeLength = totalLength / static_cast<float>(stripeCount);

        // Four cross-section points per ring: two for each curb.
        using CurbRing = std::array<aiko::vec3, 4>;

        std::vector<CurbRing> rings(ringCount + 1);

        for (size_t i = 0; i <= ringCount; ++i)
        {
            const size_t sampleIdx = i % ringCount;

            const auto& position = samples[sampleIdx];
            const auto tangent = spline.getSampleTangent(sampleIdx);

            const auto rightVector = aiko::math::cross(tangent, desc.worldUp);

            AIKO_ASSERT(aiko::math::lengthSquared(rightVector) > epsilon, "Invalid road frame");

            const auto right = aiko::math::normalize(rightVector);
            const auto up = aiko::math::normalize(aiko::math::cross(right, tangent));

            for (size_t j = 0; j < 4; ++j)
            {
                rings[i][j] = position + right * (x[j] * desc.roadWidth) + up * (y[j] + SurfaceOffset);
            }
        }

        aiko::MeshAsset asset;

        // Each section contains two independent quads.
        // Independent vertices prevent color interpolation between stripes.
        auto appendQuad = [&](const aiko::vec3& a, const aiko::vec3& b, const aiko::vec3& c, const aiko::vec3& d, float u0, float u1, const aiko::Color& color)
        {
            AIKO_ASSERT(asset.m_vertices.size() + 4 <= std::numeric_limits<uint32_t>::max(), "Curb mesh exceeds index range");

            const uint32_t base = static_cast<uint32_t>(asset.m_vertices.size());

            asset.m_vertices.insert(asset.m_vertices.end(), {a, b, c, d});

            for (size_t k = 0; k < 4; ++k)
            {
                asset.m_normals.push_back(desc.worldUp);
                asset.m_colors.push_back(color);
            }

            asset.m_textCoord.insert(asset.m_textCoord.end(), {{u0, 0.0f}, {u0, 1.0f}, {u1, 0.0f}, {u1, 1.0f}});

            // Match the existing road-strip winding.
            asset.m_indices.insert(asset.m_indices.end(), {base, base + 2, base + 1, base + 1, base + 2, base + 3});
        };

        std::vector<float> cuts;

        for (size_t i = 0; i < ringCount; ++i)
        {
            const float segmentStart = distances[i];
            const float segmentEnd = distances[i + 1];
            const float segmentLength = segmentEnd - segmentStart;

            cuts.clear();
            cuts.push_back(segmentStart);

            // Insert every stripe boundary contained in this segment.
            const size_t firstBoundary = static_cast<size_t>( std::floor(segmentStart / stripeLength) ) + 1;

            for (size_t k = firstBoundary; k < stripeCount; ++k)
            {
                const float boundary = k * stripeLength;

                if (boundary >= segmentEnd)
                {
                    break;
                }

                if (boundary > segmentStart)
                {
                    cuts.push_back(boundary);
                }
            }

            cuts.push_back(segmentEnd);

            for (size_t piece = 0; piece + 1 < cuts.size(); ++piece)
            {
                const float start = cuts[piece];
                const float end = cuts[piece + 1];

                if (end <= start)
                {
                    continue;
                }

                // Use the midpoint to determine the stripe color.
                const float midpoint = (start + end) * 0.5f;

                const size_t stripeIndex = std::min(stripeCount - 1, static_cast<size_t>( midpoint / stripeLength ));

                const aiko::Color color = stripeIndex % 2 == 0 ? Red : White;

                const float t0 = (start - segmentStart) / segmentLength;

                const float t1 = (end - segmentStart) / segmentLength;

                // Two curbs, each defined by two lateral endpoints.
                for (size_t strip = 0; strip < 2; ++strip)
                {
                    const size_t j = strip * 2;

                    const auto a = aiko::math::lerp(rings[i][j], rings[i + 1][j], t0);
                    const auto b = aiko::math::lerp(rings[i][j + 1], rings[i + 1][j + 1], t0);
                    const auto c = aiko::math::lerp(rings[i][j], rings[i + 1][j], t1);
                    const auto d = aiko::math::lerp(rings[i][j + 1], rings[i + 1][j + 1], t1);

                    appendQuad(a, b, c, d, start / stripeLength, end / stripeLength, color);
                }
            }
        }

        AIKO_ASSERT(asset.m_vertices.size() == asset.m_normals.size(), "Invalid curb mesh attributes");
        AIKO_ASSERT(asset.m_vertices.size() == asset.m_colors.size(), "Invalid curb mesh attributes");
        AIKO_ASSERT(asset.m_vertices.size() == asset.m_textCoord.size(), "Invalid curb mesh attributes");

        return asset;
    }

    float RoadMeshBuilder::getProfileTopHeight(const RoadProfile& profile, float x)
    {
        float height = std::numeric_limits<float>::lowest();
        bool found = false;

        const size_t edgeCount = profile.closed
            ? profile.points.size()
            : profile.points.size() - 1;

        for (size_t i = 0; i < edgeCount; ++i)
        {
            const auto& a = profile.points[i];
            const auto& b = profile.points[(i + 1) % profile.points.size()];

            if (x < std::min(a.x, b.x) || x > std::max(a.x, b.x))
            {
                continue;
            }

            const float dx = b.x - a.x;

            const float y = std::abs(dx) <= 1e-6f
                ? std::max(a.y, b.y)
                : a.y + (b.y - a.y) *
                    std::clamp((x - a.x) / dx, 0.0f, 1.0f);

            height = std::max(height, y);
            found = true;
        }

        AIKO_ASSERT(found, "No profile surface at requested X");
        return height;
    }
}
