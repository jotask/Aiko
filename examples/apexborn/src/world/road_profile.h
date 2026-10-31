#pragma once

#include <math/math.h>

#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace ab
{
    struct RoadProfile
    {
        std::vector<aiko::vec2> points = {
            {-0.5f,  0.0f},
            { 0.5f,  0.0f},
            { 0.5f, -0.15f},
            {-0.5f, -0.15f}
        };

        bool closed = true;

        bool normalizeWidth()
        {
            const size_t minPoints = closed ? 3 : 2;
            if (points.size() < minPoints)
            {
                return false;
            }

            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();

            for (const auto& point : points)
            {
                if (!std::isfinite(point.x) || !std::isfinite(point.y))
                {
                    return false;
                }

                minX = std::min(minX, point.x);
                maxX = std::max(maxX, point.x);
            }

            const float width = maxX - minX;
            if (!std::isfinite(width) || width <= 1e-5f)
            {
                return false;
            }

            const float center = minX + width * 0.5f;

            // Validate the normalized candidate before modifying this profile.
            RoadProfile normalized = *this;

            for (auto& point : normalized.points)
            {
                point.x = (point.x - center) / width;
            }

            const size_t edgeCount =
                closed ? normalized.points.size() : normalized.points.size() - 1;

            for (size_t i = 0; i < edgeCount; ++i)
            {
                const auto& a = normalized.points[i];
                const auto& b = normalized.points[(i + 1) % normalized.points.size()];

                if (aiko::math::distanceSquared(a, b) <= 1e-10f)
                {
                    return false;
                }
            }

            *this = std::move(normalized);
            return true;
        }

    };
}