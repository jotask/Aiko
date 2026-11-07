#include "wolf_player.h"

#include "math/math.h"

namespace wc
{

    aiko::vec2 WolfPlayer::forward() const
    {
        return { aiko::math::cos(angleRadians), aiko::math::sin(angleRadians) };
    }

    aiko::vec2 WolfPlayer::right() const
    {
        const aiko::vec2 fwd = forward();
        return { -fwd.y, fwd.x };
    }

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
