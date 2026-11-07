#include "wolf_player.h"

#include "math/math.h"

namespace wc
{

    WolfPlayer WolfPlayer::fromSpawn(const WolfPlayerSpawn& spawn)
    {
        float radians = 0.0f;
        switch (spawn.direction)
        {
            case WolfDirection::East:   radians = 0.0f; break;
            case WolfDirection::South:  radians = aiko::PI / 2.0f; break;
            case WolfDirection::West:   radians = aiko::PI; break;
            case WolfDirection::North:  radians = -aiko::PI / 2.0f; break;
                default: AIKO_ASSERT(false, "Not supported direction");
        }
        return WolfPlayer
        {
            .position = aiko::vec2( spawn.tileX + 0.5f, spawn.tileY + 0.5f ),
            .angleRadians = radians,
        };
    }
}
