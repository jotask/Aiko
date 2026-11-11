#pragma once

#include "wolf_hitscan_types.h"

#include <span>

namespace wc
{
    class WolfMap;
    class WolfHitscan final
    {
    public:
        static WolfHitscanHit trace(const WolfMap& map, const aiko::vec2& origin, const aiko::vec2& direction, float maxDistance, std::span<const WolfHitscanTarget> targets);
    };
}

