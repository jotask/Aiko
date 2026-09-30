#include <shape.h>

#include "math/math.h"

#include <unordered_map>
#include <algorithm>
#include <limits>

namespace sph
{

    Shape::Shape(aiko::vec3 position, aiko::MeshAsset asset, aiko::Color color)
        : m_position(position)
        , m_asset(std::move(asset))
        , m_color(color)
    {
        calculateBoundaryEdges();
    }

    void Shape::calculateBoundaryEdges()
    {
        struct EdgeData
        {
            size_t a = 0;
            size_t b = 0;
            size_t count = 0;
        };

        std::unordered_map<aiko::u64, EdgeData> edges;

        const auto addEdge = [&edges](size_t a, size_t b)
            {
                const size_t minIndex = std::min(a, b);
                const size_t maxIndex = std::max(a, b);

                const aiko::u64 key = (static_cast<aiko::u64>(minIndex) << 32) | static_cast<aiko::u64>(maxIndex);

                auto& edge = edges[key];

                edge.a = minIndex;
                edge.b = maxIndex;
                ++edge.count;
            };

        for (size_t i = 0; i < m_asset.m_indices.size(); i += 3)
        {
            const size_t a = m_asset.m_indices[i];
            const size_t b = m_asset.m_indices[i + 1];
            const size_t c = m_asset.m_indices[i + 2];

            addEdge(a, b);
            addEdge(b, c);
            addEdge(c, a);
        }

        m_boundaryEdges.clear();

        for (const auto& [key, edge] : edges)
        {
            if (edge.count == 1)
            {
                m_boundaryEdges.push_back(
                {
                    .a = edge.a,
                    .b = edge.b
                });
            }
        }
    }

    bool Shape::isPointInside(const aiko::vec3& worldPoint) const
    {
        const aiko::vec3 point = worldPoint - m_position;

        bool inside = false;

        for (const ShapeEdge& edge : m_boundaryEdges)
        {
            const aiko::vec3& a = m_asset.m_vertices[edge.a];
            const aiko::vec3& b = m_asset.m_vertices[edge.b];

            const bool intersects = ((a.y > point.y) != (b.y > point.y)) && (point.x < (b.x - a.x) * (point.y - a.y) / (b.y - a.y) + a.x);

            if (intersects)
            {
                inside = !inside;
            }
        }

        return inside;
    }

    void Shape::moveBy(const aiko::vec3& offset)
    {
        m_position += offset;
    }

    bool Shape::getNearestVector(const aiko::vec3& worldPoint, const float affectDistance, aiko::vec3& out) const
    {
        const aiko::vec3 localPoint = worldPoint - m_position;
        float closestDistanceSquared = std::numeric_limits<float>::max();

        aiko::vec3 closestPoint = {};

        for (const ShapeEdge& edge : m_boundaryEdges)
        {
            const aiko::vec3& a = m_asset.m_vertices[edge.a];
            const aiko::vec3& b = m_asset.m_vertices[edge.b];

            const aiko::vec3 pointOnEdge = closestPointOnSegment(localPoint, a, b);

            const aiko::vec3 direction = pointOnEdge - localPoint;
            const float distanceSquared = aiko::math::lengthSquared(direction);

            if (distanceSquared < closestDistanceSquared)
            {
                closestDistanceSquared = distanceSquared;
                closestPoint = pointOnEdge;
            }
        }

        if (closestDistanceSquared > affectDistance * affectDistance)
        {
            out = {};
            return false;
        }

        out = closestPoint - localPoint;

        return true;
    }

    bool Shape::getDirectionOut(const aiko::vec3& worldPoint, float radius, aiko::vec3& out) const
    {
        const aiko::vec3 localPoint = worldPoint - m_position;
        float closestDistanceSquared = std::numeric_limits<float>::max();

        aiko::vec3 closestPoint = {};

        for (const ShapeEdge& edge : m_boundaryEdges)
        {
            const aiko::vec3& a = m_asset.m_vertices[edge.a];
            const aiko::vec3& b = m_asset.m_vertices[edge.b];

            const aiko::vec3 pointOnEdge = closestPointOnSegment(localPoint, a, b);
            const aiko::vec3 direction = pointOnEdge - localPoint;

            const float distanceSquared = aiko::math::lengthSquared(direction);

            if (distanceSquared < closestDistanceSquared)
            {
                closestDistanceSquared = distanceSquared;
                closestPoint = pointOnEdge;
            }
        }

        const aiko::vec3 directionToBoundary = closestPoint - localPoint;
        const float distance = aiko::math::length(directionToBoundary);
        const bool inside = isPointInside(worldPoint);

        if (!inside && distance >= radius)
        {
            out = {};
            return false;
        }

        if (distance <= 1e-6f)
        {
            out = {};
            return false;
        }

        const aiko::vec3 normal = directionToBoundary / distance;

        if (inside)
        {
            out = directionToBoundary + normal * radius;
        }
        else
        {
            out = normal * (distance - radius);
        }

        return true;
    }

    aiko::vec3 Shape::closestPointOnSegment(const aiko::vec3& point, const aiko::vec3& a, const aiko::vec3& b)
    {
        const aiko::vec3 ab = b - a;

        const float lengthSquared = aiko::math::dot(ab, ab);

        if (lengthSquared <= 1e-6f)
        {
            return a;
        }

        const float t = aiko::math::clamp(aiko::math::dot(point - a, ab) / lengthSquared, 0.0f, 1.0f);

        return a + ab * t;
    }

}
