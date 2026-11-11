#pragma once
#include "math/math_vector.h"

namespace wc
{

    enum class WolfRaySide : uint8_t
    {
        Vertical,
        Horizontal,
    };

    enum class WolfRayHitKind
    {
        Wall,
        Door,
        PushWall,
    };

    enum class WolfRaycastMode
    {
        Render,
        Interaction,
    };

    struct WolfRayHit
    {
        bool hit = false;
        aiko::vec2 position = { 0.0f };
        float distance = 0.0f;
        int32_t tileX = 0;
        int32_t tileY = 0;
        WolfRaySide side = WolfRaySide::Horizontal;
        WolfRayHitKind kind = WolfRayHitKind::Wall;
    };
}
