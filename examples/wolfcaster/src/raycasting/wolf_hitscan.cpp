#include "wolf_hitscan.h"

#include "math/math.h"
#include "wolf_raycaster.h"

#include <algorithm>
#include <cmath>

namespace wc
{
    WolfHitscanHit WolfHitscan::trace(const WolfMap& map, const aiko::vec2& origin, const aiko::vec2& direction, float maxDistance, std::span<const WolfHitscanTarget> targets)
    {
        if(maxDistance <= 0.0f)
        {
            return {};
        }
        if (aiko::math::length(direction) == 0.0f )
        {
            return {};
        }

        WolfHitscanHitKind hitKind = WolfHitscanHitKind::None;
        size_t targetId = 0;

        float currentClosestHit = maxDistance;

        const aiko::vec2 normalizedDir = aiko::math::normalize(direction);
        const auto hit = WolfRaycaster::castRay(map, origin, normalizedDir, WolfRaycastMode::Render);

        if (hit.hit == true && hit.distance <= maxDistance)
        {
            currentClosestHit = hit.distance;
            hitKind = WolfHitscanHitKind::World;
        }

        for (size_t i = 0 ; i < targets.size(); ++i)
        {
            const WolfHitscanTarget& target = targets[i];
            const aiko::vec2 dirTo = target.position - origin;

            const float projection = aiko::math::dot(dirTo, normalizedDir);
            const float perpendicularDistanceSquared = aiko::math::dot(dirTo, dirTo) - projection * projection;

            if (perpendicularDistanceSquared > target.radius * target.radius)
            {
                continue;
            }

            const float halfChord = std::sqrt(std::max(0.0f, (target.radius * target.radius) - perpendicularDistanceSquared));
            const float entry = projection - halfChord;
            const float exit = projection + halfChord;

            if (exit < 0)
            {
                continue;
            }

            const float intersectionDistance = std::max(0.0f, entry);
            if (intersectionDistance < currentClosestHit)
            {
                currentClosestHit = intersectionDistance;
                hitKind = WolfHitscanHitKind::Actor;
                targetId = target.id;
            }

        }

        if (hitKind == WolfHitscanHitKind::None)
        {
            return {};
        }

        return WolfHitscanHit{
            .kind = hitKind,
            .distance = currentClosestHit,
            .targetId = targetId,
        };

    }
}