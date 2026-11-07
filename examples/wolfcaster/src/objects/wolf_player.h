#pragma once
#include "map/wolf_map_types.h"
#include "math/math_vector.h"

namespace wc
{

    struct WolfPlayer
    {
        aiko::vec2 position = {0.0f};
        float angleRadians = 0.0f;

        aiko::vec2 forward() const;
        aiko::vec2 right() const;

        static WolfPlayer fromSpawn(const WolfPlayerSpawn& spawn);
    };

}

