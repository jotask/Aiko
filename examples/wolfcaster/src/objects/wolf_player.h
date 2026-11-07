#pragma once
#include "map/wolf_map_types.h"
#include "math/math_vector.h"

namespace wc
{

    struct WolfPlayer
    {
        aiko::vec2 position = {0.0f};
        float angleRadians = 0.0f;
        static WolfPlayer fromSpawn(const WolfPlayerSpawn& spawn);
    };

}

