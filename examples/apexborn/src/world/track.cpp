#include "track.h"

#include "constant_types.h"
#include "core/transform.h"
#include "layers/contexts/asset_context.h"
#include "road_mesh_builder.h"
#include "track_generator.h"

#include <cmath>

namespace ab
{

    Track::Track() = default;

    Track::~Track() = default;

    void Track::init(aiko::AssetContext& assets)
    {
        m_roadMaterial.m_shaderId = assets.loadShader("model");
        m_roadMaterial.m_lit = true;
        m_roadMaterial.m_useVertexColor = true;
        m_roadMaterial.m_baseColor = aiko::WHITE;

        m_edgeMarkingsMaterial.m_shaderId = assets.loadShader("model");
        m_edgeMarkingsMaterial.m_lit = false;
        m_edgeMarkingsMaterial.m_useVertexColor = false;
        m_edgeMarkingsMaterial.m_baseColor = aiko::RAYWHITE;

        m_curbMaterial.m_shaderId = assets.loadShader("model");
        m_curbMaterial.m_lit = false;
        m_curbMaterial.m_useVertexColor = true;
        m_curbMaterial.m_baseColor = aiko::WHITE;
    }

    bool Track::generate(const TrackGeneratorSettings& settings)
    {

        if (!aiko::math::isfinite(settings.trackWidth) || settings.trackWidth <= 0.0f)
        {
            return false;
        }

        TrackGenerator::TrackGenerationDebug debug;

        const auto result = TrackGenerator::generate(settings, debug);
        if (result == std::nullopt)
        {
            return false;
        }

        m_centerLine.generate(*result);
        m_trackNavigation.recalculate(m_centerLine);

        m_trackWidth = settings.trackWidth;

        const RoadMeshBuilder::RoadMeshBuilderDesc desc = {
            .roadWidth = m_trackWidth,
            .worldUp = WorldUp,
        };

        auto road = RoadMeshBuilder::build(m_centerLine, m_profile, desc);

        m_roadMesh.upload(road.mesh);

        m_leftBoundary = std::move(road.leftBoundary);
        m_rightBoundary = std::move(road.rightBoundary);

        const auto markings = RoadMeshBuilder::buildEdgeMarkings(m_centerLine, m_profile, desc);
        m_edgeMarkingsMesh.upload(markings);

        const auto curbs = RoadMeshBuilder::buildCurbs(m_centerLine, m_profile, desc);
        m_curbMesh.upload(curbs);

        m_generationDebug = std::move(debug);
        return true;

    }

    bool Track::isOnRoad(const TrackNavigation::TrackProjection& projection) const
    {
        const float halfWidth = m_trackWidth * 0.5f;
        return std::abs(projection.signedLateralOffset) <= halfWidth;
    }

    bool Track::isOnRoad(const aiko::vec3& point) const
    {
        const auto projection = m_trackNavigation.getTrackProjectionFromPoint(point);
        return isOnRoad(projection);
    }

    TrackNavigation::TrackProjection Track::projectPoint(const aiko::vec3& point) const
    {
        return m_trackNavigation.getTrackProjectionFromPoint(point);
    }

    void Track::render(aiko::RenderContext& context)
    {
        aiko::Transform transform{};
        if (m_roadMesh.isValid())
        {
            context.drawMesh(transform, m_roadMesh, m_roadMaterial);
        }
        if (m_edgeMarkingsMesh.isValid())
        {
            context.drawMesh(transform, m_edgeMarkingsMesh, m_edgeMarkingsMaterial);
        }
        if (m_curbMesh.isValid())
        {
            context.drawMesh(transform, m_curbMesh, m_curbMaterial);
        }

        if (m_debugEnabled == true)
        {
            renderDebug(context);
        }
    }

    Track::FinishLine Track::getFinishLine() const
    {
        const auto& samples = m_centerLine.getSamplePoints();
        AIKO_ASSERT(samples.size() >= 3, "Finish line requires at least three spline samples");
        return {
            .position = samples.front(),
            .forward = m_centerLine.getSampleTangent(0),
        };
    }

    void Track::renderDebug(aiko::RenderContext& context)
    {
        m_centerLine.render(context);

        // Area size render
        {
            const auto halfWidth  = m_generationDebug.areaSize.x * 0.5f;
            const auto halfHeight = m_generationDebug.areaSize.y * 0.5f;

            const aiko::vec3 tl = {-halfWidth, 0.0f, -halfHeight};
            const aiko::vec3 tr = { halfWidth, 0.0f, -halfHeight};
            const aiko::vec3 br = { halfWidth, 0.0f,  halfHeight};
            const aiko::vec3 bl = {-halfWidth, 0.0f,  halfHeight};

            context.drawLine(tl, tr, aiko::RED);
            context.drawLine(tr, br, aiko::RED);
            context.drawLine(br, bl, aiko::RED);
            context.drawLine(bl, tl, aiko::RED);

        }

        // TrackGenerator::TrackGenerationDebug
        {
            const auto toWorld = [](const aiko::vec2& p, float height)
            {
                return aiko::vec3{p.x, height, p.y};
            };

            // Original random points (white)
            for (const auto& point : m_generationDebug.randomPoints)
            {
                context.drawPoint(toWorld(point, 0.1f), aiko::WHITE);
            }

            // Convex hull (red)
            const auto& hull = m_generationDebug.convexHull;

            for (size_t i = 0; i < hull.size(); ++i)
            {
                const auto& current = hull[i];
                const auto& next = hull[(i + 1) % hull.size()];

                context.drawLine(toWorld(current, 0.15f), toWorld(next, 0.15f), aiko::RED);
                context.drawPoint(toWorld(current, 0.2f), aiko::GREEN);
            }

            // Displaced points (cyan)
            const auto& points = m_generationDebug.displacedPoints;

            for (size_t i = 0; i < points.size(); ++i)
            {
                const auto& A = points[i];
                const auto& B = points[(i + 1) % points.size()];

                context.drawLine( {A.x, 0.2f, A.y}, {B.x, 0.2f, B.y}, aiko::CYAN);
            }

            // Corrected points
            const auto& corrected = m_generationDebug.correctedPoints;
            for (size_t i = 0; i < corrected.size(); ++i)
            {
                const auto& a = corrected[i];
                const auto& b = corrected[(i + 1) % corrected.size()];

                context.drawLine(
                    {a.x, 0.25f, a.y},
                    {b.x, 0.25f, b.y},
                    aiko::YELLOW
                );
            }
        }

        // Render boundaries
        {
            auto drawBoundary = [&context](const Spline::Points& points, const aiko::Color& color)
            {
                if (points.size() < 2)
                {
                    return;
                }

                const aiko::vec3 offset = {0.0f, 0.04f, 0.0f};

                for (size_t i = 0; i < points.size(); ++i)
                {
                    const auto& a = points[i];
                    const auto& b = points[(i + 1) % points.size()];
                    context.drawLine(a + offset, b + offset, color);
                }
            };

            drawBoundary(m_leftBoundary, aiko::CYAN);
            drawBoundary(m_rightBoundary, aiko::MAGENTA);
        }

    }

}
