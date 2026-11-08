#include "wolf_player.h"

#include "map/wolf_map.h"
#include "math/math.h"

namespace wc
{

    aiko::vec2 WolfPlayer::forward() const
    {
        return { aiko::math::cos(m_angleRadians), aiko::math::sin(m_angleRadians) };
    }

    aiko::vec2 WolfPlayer::right() const
    {
        const aiko::vec2 fwd = forward();
        return { -fwd.y, fwd.x };
    }

    void WolfPlayer::move(const aiko::vec2 displacement, const WolfMap& map)
    {
        const aiko::vec2 candidatePositionX = m_position + aiko::vec2(displacement.x, 0);
        if (map.collidesAt(candidatePositionX, m_radius) == false)
        {
            m_position.x = candidatePositionX.x;
        }

        const aiko::vec2 candidatePositionY = m_position + aiko::vec2(0, displacement.y);
        if (map.collidesAt(candidatePositionY, m_radius) == false)
        {
            m_position.y = candidatePositionY.y;
        }
    }

    void WolfPlayer::rotate(float angle)
    {
        m_angleRadians += angle;
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
        WolfPlayer player;
        player.m_position = aiko::vec2( spawn.tileX + 0.5f, spawn.tileY + 0.5f );
        player.m_angleRadians = radians;
        return player;
    }

}
