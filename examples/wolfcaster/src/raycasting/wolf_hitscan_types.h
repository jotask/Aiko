#pragma once

#include "math/math_vector.h"

#include <cstddef>

namespace wc
{

    enum class WolfHitscanHitKind
    {
        None,
        World,
        Actor,
    };

    struct WolfHitscanHit
    {
        WolfHitscanHitKind kind = WolfHitscanHitKind::None;
        float distance = 0.0f;
        size_t targetId = 0;
    };

    struct WolfHitscanTarget
    {
        aiko::vec2 position = {0};
        float radius = 0.2f;
        size_t id = 0;
    };
}

