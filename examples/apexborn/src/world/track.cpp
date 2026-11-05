#include "track.h"

#include "constant_types.h"
#include "core/transform.h"
#include "layers/contexts/asset_context.h"
#include "road_mesh_builder.h"
#include "track_generator.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace ab
{

    namespace
    {
        float cross2D(const aiko::vec2& a, const aiko::vec2& b)
        {
            return a.x * b.y - a.y * b.x;
        }
    }

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

        rebuildBoundaryGrid();

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

        const auto forward = m_centerLine.getSampleTangent(0);
        const auto right = aiko::math::normalize(aiko::math::cross(forward, WorldUp));
        const auto up = aiko::math::normalize(aiko::math::cross(right, forward));

        const float surfaceHeight = RoadMeshBuilder::getProfileTopHeight(m_profile, 0.0f);
        return {
            .position = samples.front() + up * surfaceHeight,
            .forward = forward
        };
    }

    std::optional<Track::RoadRaycastHit> Track::raycastBoundary(const aiko::vec3& origin, const aiko::vec3& direction, float maxDistance) const
    {
        if (!std::isfinite(maxDistance) || maxDistance <= 0.0f)
        {
            return std::nullopt;
        }

        const auto& grid = m_boundaryGrid;

        if (grid.cells.empty())
        {
            return std::nullopt;
        }

        const aiko::vec2 rayOrigin = {origin.x, origin.z};
        const aiko::vec2 horizontalDirection = { direction.x, direction.z };

        if (!aiko::math::isfinite(rayOrigin.x) || !aiko::math::isfinite(rayOrigin.y))
        {
            return std::nullopt;
        }

        const float lengthSquared = aiko::math::lengthSquared(horizontalDirection);

        if (!std::isfinite(lengthSquared) || lengthSquared <= epsilon * epsilon)
        {
            return std::nullopt;
        }

        const aiko::vec2 rayDirection = horizontalDirection / std::sqrt(lengthSquared);

        const aiko::vec2 rayEnd = rayOrigin + rayDirection * maxDistance;

        if (!aiko::math::isfinite(rayEnd.x) || !aiko::math::isfinite(rayEnd.y))
        {
            return std::nullopt;
        }

        // AABB of the finite ray.
        const float rayMinX = std::min(rayOrigin.x, rayEnd.x);
        const float rayMaxX = std::max(rayOrigin.x, rayEnd.x);
        const float rayMinZ = std::min(rayOrigin.y, rayEnd.y);
        const float rayMaxZ = std::max(rayOrigin.y, rayEnd.y);

        const float gridMaxX = grid.origin.x + grid.columns * grid.cellSize;

        const float gridMaxZ = grid.origin.y + grid.rows * grid.cellSize;

        // Ray is completely outside the indexed area.
        if (rayMaxX < grid.origin.x || rayMinX > gridMaxX || rayMaxZ < grid.origin.y || rayMinZ > gridMaxZ)
        {
            return std::nullopt;
        }

        const auto toColumn = [&](float x)
        {
            return std::clamp(static_cast<int>(std::floor((x - grid.origin.x) / grid.cellSize)), 0, grid.columns - 1);
        };

        const auto toRow = [&](float z)
        {
            return std::clamp(static_cast<int>( std::floor((z - grid.origin.y) / grid.cellSize) ), 0, grid.rows - 1);
        };

        // Clamp the query to the grid before converting to cells.
        const int minColumn = toColumn( std::max(rayMinX, grid.origin.x) );

        const int maxColumn = toColumn( std::min(rayMaxX, gridMaxX) );

        const int minRow = toRow( std::max(rayMinZ, grid.origin.y) );

        const int maxRow = toRow( std::min(rayMaxZ, gridMaxZ) );

        std::optional<RoadRaycastHit> closestHit;
        float closestDistance = maxDistance;

        for (int row = minRow; row <= maxRow; ++row)
        {
            for (int column = minColumn; column <= maxColumn; ++column)
            {
                const size_t cellIndex = static_cast<size_t>(row) * grid.columns + column;

                for (const size_t segmentIndex : grid.cells[cellIndex])
                {
                    const auto& segment = grid.segments[segmentIndex];

                    const aiko::vec2 a = { segment.start.x, segment.start.z };

                    const aiko::vec2 b = { segment.end.x, segment.end.z };

                    const aiko::vec2 edge = b - a;
                    const aiko::vec2 offset = a - rayOrigin;

                    const float denominator = cross2D(rayDirection, edge);

                    // Preserve the original intersection behavior.
                    if (std::abs(denominator) <= 1e-6f)
                    {
                        continue;
                    }

                    const float t = cross2D(offset, edge) / denominator;
                    const float u = cross2D(offset, rayDirection) / denominator;

                    if (t < 0.0f || t > closestDistance)
                    {
                        continue;
                    }

                    if (u < 0.0f || u > 1.0f)
                    {
                        continue;
                    }

                    closestDistance = t;

                    closestHit = RoadRaycastHit{
                        .position = segment.start + (segment.end - segment.start) * u,
                        .distance = t
                    };
                }
            }
        }

        return closestHit;
    }

    void Track::rebuildBoundaryGrid()
    {
        BoundaryGrid grid;

        grid.cellSize = std::max(2.0f, m_trackWidth);

        const auto addBoundary = [&](const Spline::Points& points)
        {
            if (points.size() < 2)
            {
                return;
            }

            for (size_t i = 0; i < points.size(); ++i)
            {
                grid.segments.push_back({
                    points[i],
                    points[(i + 1) % points.size()]
                });
            }
        };

        addBoundary(m_leftBoundary);
        addBoundary(m_rightBoundary);

        if (grid.segments.empty())
        {
            m_boundaryGrid = std::move(grid);
            return;
        }

        // Find the bounds of all boundary segments.
        float minX = std::numeric_limits<float>::max();
        float minZ = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float maxZ = std::numeric_limits<float>::lowest();

        const auto expandBounds = [&](const aiko::vec3& point)
        {
            minX = std::min(minX, point.x);
            minZ = std::min(minZ, point.z);
            maxX = std::max(maxX, point.x);
            maxZ = std::max(maxZ, point.z);
        };

        for (const auto& segment : grid.segments)
        {
            expandBounds(segment.start);
            expandBounds(segment.end);
        }

        // Add one cell of padding around the track.
        grid.origin = { minX - grid.cellSize, minZ - grid.cellSize};

        grid.columns = std::max(1, static_cast<int>(     std::ceil((maxX - minX) / grid.cellSize) ) + 2);

        grid.rows = std::max(1, static_cast<int>(     std::ceil((maxZ - minZ) / grid.cellSize) ) + 2);

        grid.cells.resize(static_cast<size_t>(grid.columns) * static_cast<size_t>(grid.rows));

        const auto toColumn = [&](float x)
        {
            return std::clamp(static_cast<int>(std::floor((x - grid.origin.x) / grid.cellSize)), 0, grid.columns - 1);
        };

        const auto toRow = [&](float z)
        {
            return std::clamp(static_cast<int>(std::floor((z - grid.origin.y) / grid.cellSize)), 0, grid.rows - 1);
        };

        // Insert every segment into overlapping cells.
        for (size_t i = 0; i < grid.segments.size(); ++i)
        {
            const auto& segment = grid.segments[i];

            const int minColumn = toColumn(std::min(segment.start.x, segment.end.x));

            const int maxColumn = toColumn(std::max(segment.start.x, segment.end.x));

            const int minRow = toRow(std::min(segment.start.z, segment.end.z));

            const int maxRow = toRow(std::max(segment.start.z, segment.end.z));

            for (int row = minRow; row <= maxRow; ++row)
            {
                for (int column = minColumn; column <= maxColumn; ++column)
                {
                    const size_t index = static_cast<size_t>(row) * grid.columns + column;
                    grid.cells[index].push_back(i);
                }
            }
        }

        m_boundaryGrid = std::move(grid);
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
