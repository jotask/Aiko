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

        const WolfRayHit pushWallHit = intersectPushWall(wolfMap, origin, normalizedDirection);

        const auto nearestHit = [&](const WolfRayHit& staticHit) -> WolfRayHit
        {
            if (pushWallHit.hit && (!staticHit.hit || pushWallHit.distance < staticHit.distance))
            {
                return pushWallHit;
            }
            return staticHit;
        };

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
            return nearestHit(doorHit);
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
                return nearestHit({});
            };

            const WolfRayHit doorHitDDA = intersectDoor(wolfMap, map.x, map.y, origin, normalizedDirection, hitDistance, std::min(sideDistance.x, sideDistance.y), mode);
            if (doorHitDDA.hit == true)
            {
                return nearestHit(doorHitDDA);
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
            return nearestHit(hit);

        }

        return nearestHit({});

    }

    WolfRayHit WolfRaycaster::intersectDoor(const WolfMap& map, int32_t tileX, int32_t tileY, const aiko::vec2& rayOrigin, const aiko::vec2& rayDirection, float entryDistance, float exitDistance, WolfRaycastMode mode)
    {
        AIKO_ASSERT(tileX >= 0 && tileX < map.getWidth() && tileY >= 0 && tileY < map.getHeight(), "Outside of map OOB");
        const TileId tileId = map.getGeometryTile(tileX, tileY);
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

    WolfRayHit WolfRaycaster::intersectPushWall(const WolfMap& map, const aiko::vec2& origin, const aiko::vec2& direction)
    {
        const WolfDoor* wall = map.getActivePushWall();

        if (wall == nullptr)
        {
            return {};
        }

        const aiko::vec2 boxMin = {
            float(wall->tileCoord.x) + wall->pushDirection.x * wall->openProgress,
            float(wall->tileCoord.y) + wall->pushDirection.y * wall->openProgress
        };

        const aiko::vec2 boxMax = boxMin + aiko::vec2(1.0f);

        float nearDistance = -std::numeric_limits<float>::infinity();
        float farDistance = std::numeric_limits<float>::infinity();

        WolfRaySide nearSide = WolfRaySide::Horizontal;
        WolfRaySide farSide = WolfRaySide::Horizontal;

        for (int axis = 0; axis < 2; ++axis)
        {
            const float rayOrigin = axis == 0 ? origin.x : origin.y;
            const float rayDir = axis == 0 ? direction.x : direction.y;

            const float minimum = axis == 0 ? boxMin.x : boxMin.y;
            const float maximum = axis == 0 ? boxMax.x : boxMax.y;

            if (std::abs(rayDir) < 0.000001f)
            {
                if (rayOrigin < minimum || rayOrigin > maximum)
                {
                    return {};
                }

                continue;
            }

            const float t1 = (minimum - rayOrigin) / rayDir;
            const float t2 = (maximum - rayOrigin) / rayDir;

            const float axisNear = std::min(t1, t2);
            const float axisFar = std::max(t1, t2);

            const WolfRaySide side = axis == 0 ? WolfRaySide::Vertical : WolfRaySide::Horizontal;

            if (axisNear > nearDistance)
            {
                nearDistance = axisNear;
                nearSide = side;
            }

            if (axisFar < farDistance)
            {
                farDistance = axisFar;
                farSide = side;
            }

            if (nearDistance > farDistance)
            {
                return {};
            }
        }

        if (farDistance < 0.0f)
        {
            return {};
        }

        const bool entering = nearDistance >= 0.0f;
        const float hitDistance = entering ? nearDistance : farDistance;
        const WolfRaySide hitSide = entering ? nearSide : farSide;

        return WolfRayHit{
            .hit = true,
            .position = origin + direction * hitDistance,
            .distance = hitDistance,
            .tileX = int32_t(wall->tileCoord.x),
            .tileY = int32_t(wall->tileCoord.y),
            .side = hitSide,
            .kind = WolfRayHitKind::PushWall
        };
    }
}
