#pragma once
#include "wolf_raycaster_types.h"

namespace wc
{
    class WolfMap;
    class WolfRaycaster final
    {
    public:
        static WolfRayHit castRay(const WolfMap& map, const aiko::vec2& origin, const aiko::vec2& direction);
    private:
        static WolfRayHit intersectDoor(const WolfMap& map, int32_t tileX, int32_t tileY, const aiko::vec2& rayOrigin, const aiko::vec2& rayDirection, float entryDistance, float exitDistance);
    };

}
