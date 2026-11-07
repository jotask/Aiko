#pragma once
#include "wolf_raycaster_types.h"

namespace wc
{
    class WolfMap;
    class WolfRaycaster final
    {
    public:
        static WolfRayHit castRay(const WolfMap& map, const aiko::vec2& origin, const aiko::vec2& direction);
    };

}
