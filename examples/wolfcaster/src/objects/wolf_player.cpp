#include "wolf_player.h"

#include "map/wolf_map.h"
#include "math/math.h"

#include <algorithm>

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

    void WolfPlayer::updateWeapon(float deltaTime, bool triggerPressed)
    {

        constexpr float ShotCooldown = 0.4f;

        m_shotThisFrame = false;

        m_fireCooldownRemaining = std::max(0.0f, m_fireCooldownRemaining - deltaTime);

        if (triggerPressed == true)
        {
            if (m_fireCooldownRemaining == 0.0f)
            {
                m_shotThisFrame = true;
                m_fireCooldownRemaining = ShotCooldown;
            }
        }
    }

    void WolfPlayer::takeDamage(int32_t damage)
    {
        if (isAlive() == false || damage <= 0)
        {
            return;
        }
        m_health = std::max(0, m_health - damage);
    }

    void WolfPlayer::move(const aiko::vec2 displacement, const WolfMap& map)
    {
        const aiko::vec2 candidatePositionX = m_position + aiko::vec2(displacement.x, 0);
        if (map.collidesAt(candidatePositionX, PlayerRadius) == false)
        {
            m_position.x = candidatePositionX.x;
        }

        const aiko::vec2 candidatePositionY = m_position + aiko::vec2(0, displacement.y);
        if (map.collidesAt(candidatePositionY, PlayerRadius) == false)
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
        player.m_position = aiko::vec2( spawn.tile.x + 0.5f, spawn.tile.y + 0.5f );
        player.m_angleRadians = radians;
        return player;
    }

}
