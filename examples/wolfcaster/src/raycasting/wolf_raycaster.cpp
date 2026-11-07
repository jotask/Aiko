#include "wolf_raycaster.h"

#include "map/wolf_map.h"

#include "math/math.h"

#include <cmath>
#include <limits>

namespace wc
{

    WolfRayHit WolfRaycaster::castRay(const WolfMap& wolfMap, const aiko::vec2& origin, const aiko::vec2& direction)
    {

        if (aiko::math::length(direction) < std::numeric_limits<float>::epsilon() )
        {
            return {};
        }

        // Identify starting tile
        aiko::vec2def<int32_t> map = {
            static_cast<int32_t>(std::floor(origin.x)),
            static_cast<int32_t>(std::floor(origin.y)),
        };

        // calculate step direction
        aiko::vec2def<int32_t> stepDir = { 0 };
        if (direction.x < 0)
        {
            stepDir.x = -1;
        }
        else
        {
            stepDir.x = 1;
        }

        if (direction.y < 0)
        {
            stepDir.y = -1;
        }
        else
        {
            stepDir.y = 1;
        }

        // Calculate delta distance
        const aiko::vec2 normalizedDirection = aiko::math::normalize(direction);

        aiko::vec2 deltaDistance = {
            (normalizedDirection.x != 0) ? std::abs(1 / normalizedDirection.x) : std::numeric_limits<float>::infinity(),
            (normalizedDirection.y != 0) ? std::abs(1 / normalizedDirection.y) : std::numeric_limits<float>::infinity(),
        };

        auto calculateSideDistance = [](int32_t stepDir, float map, float origin, float deltaDst) -> float
        {
            if (std::isinf(deltaDst)) return std::numeric_limits<float>::infinity();
            if (stepDir == 1)  { return (map + 1 - origin) * deltaDst; }
            if (stepDir == -1) { return (origin - map) * deltaDst; }
            return 0.0f;
        };
        aiko::vec2 sideDistance = {
            calculateSideDistance(stepDir.x, map.x, origin.x, deltaDistance.x),
            calculateSideDistance(stepDir.y, map.y, origin.y, deltaDistance.y),
        };

        float hitDistance = 0.0f;
        WolfRaySide hitSide = WolfRaySide::Horizontal;

        const int32_t maxSteps = wolfMap.getWidth() + wolfMap.getHeight() + 1;

        for(int32_t step = 0 ; step < maxSteps ; ++step)
        {
            if (sideDistance.x < sideDistance.y)
            {
                hitDistance = sideDistance.x;
                map.x += stepDir.x;
                sideDistance.x += deltaDistance.x;
                hitSide = WolfRaySide::Vertical;
            }
            else
            {
                hitDistance = sideDistance.y;
                map.y += stepDir.y;
                sideDistance.y += deltaDistance.y;
                hitSide = WolfRaySide::Horizontal;
            }

            if (wolfMap.isBlockingTile(map.x, map.y) == false)
            {
                continue;
            }

            const WolfRayHit hit
            {
                .hit = true,
                .position = origin + normalizedDirection * hitDistance,
                .distance = hitDistance,
                .tileX = map.x,
                .tileY = map.y,
                .side = hitSide,
            };
            return hit;

        }

        return {};

    }
}
