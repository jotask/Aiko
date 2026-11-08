#include "wolf_raycaster.h"

#include "map/wolf_map.h"

#include "math/math.h"

#include <cmath>
#include <limits>

namespace wc
{

    WolfRayHit WolfRaycaster::castRay(const WolfMap& wolfMap, const aiko::vec2& origin, const aiko::vec2& direction, WolfRaycastMode mode)
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

        const WolfRayHit doorHit = intersectDoor(wolfMap, map.x, map.y, origin, normalizedDirection, 0.0f, std::min(sideDistance.x, sideDistance.y), mode);
        if (doorHit.hit == true)
        {
            return doorHit;
        }

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

            if(map.x <0 || map.x >= wolfMap.getWidth() || map.y < 0 || map.y >= wolfMap.getHeight())
            {
                // out of the map
                return {};
            };

            const WolfRayHit doorHitDDA = intersectDoor(wolfMap, map.x, map.y, origin, normalizedDirection, hitDistance, std::min(sideDistance.x, sideDistance.y), mode);
            if (doorHitDDA.hit == true)
            {
                return doorHitDDA;
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
                .kind = WolfRayHitKind::Wall,
            };
            return hit;

        }

        return {};

    }

    WolfRayHit WolfRaycaster::intersectDoor(const WolfMap& map, int32_t tileX, int32_t tileY, const aiko::vec2& rayOrigin, const aiko::vec2& rayDirection, float entryDistance, float exitDistance, WolfRaycastMode mode)
    {
        AIKO_ASSERT(tileX >= 0 && tileX < map.getWidth() && tileY >= 0 && tileY < map.getHeight(), "Outside of map OOB");
        const uint16_t tileId = map.getTile(WolfMapPlane::Geometry, tileX, tileY);
        if (tileId < 90 || tileId > 101)
        {
            return {};
        }

        const WolfRaySide doorOrientation = (tileId % 2 == 0 ) ? WolfRaySide::Vertical : WolfRaySide::Horizontal;

        float doorDistance = 0.0f;

        switch (doorOrientation)
        {
            case WolfRaySide::Horizontal:
                if (rayDirection.y == 0.0f)
                {
                    return{};
                }
                doorDistance = (tileY + 0.5f - rayOrigin.y) / rayDirection.y;
                break;
            case WolfRaySide::Vertical:
                if (rayDirection.x == 0.0f)
                {
                    return{};
                }
                doorDistance = (tileX + 0.5f - rayOrigin.x) / rayDirection.x;
                break;
            default:
                AIKO_ASSERT(false, "Unknown orientation");
        }

        if (aiko::math::isfinite(doorDistance) == false || doorDistance < entryDistance || doorDistance >= exitDistance)
        {
            return {};
        }

        const WolfDoor* door = map.getDoor(tileX, tileY);
        AIKO_ASSERT(door != nullptr, "No door found after hitting door?");

        if (door->openProgress >= 1.0f && mode == WolfRaycastMode::Render)
        {
            return {};
        }

        const aiko::vec2 hitPosition = rayOrigin + rayDirection * doorDistance;

        float localPosition = 0.0f;

        switch (doorOrientation)
        {
            case WolfRaySide::Horizontal:  localPosition = hitPosition.x - tileX; break;
            case WolfRaySide::Vertical: localPosition = hitPosition.y - tileY; break;
            default: AIKO_ASSERT(false, "Unknown");
        }

        localPosition = aiko::math::clamp(localPosition, 0.0f, 1.0f);

        if (localPosition < door->openProgress && mode == WolfRaycastMode::Render)
        {
            return {};
        }

        const WolfRayHit hit = {
            .hit = true,
            .position = hitPosition,
            .distance = doorDistance,
            .tileX = tileX,
            .tileY = tileY,
            .side = doorOrientation,
            .kind = WolfRayHitKind::Door,
        };
        return hit;

    }
}
