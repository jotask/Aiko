#include "wolf_player.h"

#include "map/wolf_map.h"
#include "math/math.h"
#include "wolf_weapon_stats.h"
#include "wolfcaster_types.h"

#include <algorithm>

namespace wc
{

    void WolfPlayer::setWeapon(WolfWeapon weapon)
    {
        if (weapon == m_currentWeapon)
        {
            return;
        }
        m_currentWeapon = weapon;
        m_weaponAnimationFrame = 0;
        m_weaponAnimationTime = 0.0f;
    }

    aiko::vec2 WolfPlayer::forward() const
    {
        return { aiko::math::cos(m_angleRadians), aiko::math::sin(m_angleRadians) };
    }

    aiko::vec2 WolfPlayer::right() const
    {
        const aiko::vec2 fwd = forward();
        return { -fwd.y, fwd.x };
    }

    void WolfPlayer::updateWeapon(float deltaTime, bool triggerHeld, bool triggerJustPressed)
    {
        constexpr uint8_t LastWeaponAnimationFrame = 4;

        const WolfWeaponStat& weaponStat = getWolfWeaponStats(m_currentWeapon);

        m_shotsThisFrame = 0;

        if (isAlive() == false)
        {
            m_weaponAnimationFrame = 0;
            m_weaponAnimationTime = 0.0f;
            return;
        }

        AIKO_ASSERT(weaponStat.fireIntervalSeconds > 0.0f, "Invalid weapon fire interval");

        if (weaponStat.automatic == true && triggerHeld == true)
        {
            m_fireCooldownRemaining -= deltaTime;
            while (m_fireCooldownRemaining <= 0.0f)
            {
                if (weaponStat.ammoCost > 0 && consumeAmmo() == false)
                {
                    m_fireCooldownRemaining = 0.0f;
                    break;
                }
                ++m_shotsThisFrame;
                m_fireCooldownRemaining += weaponStat.fireIntervalSeconds;
            }
        }
        else
        {
            m_fireCooldownRemaining = std::max( 0.0f, m_fireCooldownRemaining - deltaTime);

            if (weaponStat.automatic == false && triggerJustPressed && m_fireCooldownRemaining <= 0.0f)
            {
                if (weaponStat.ammoCost == 0 || consumeAmmo())
                {
                    ++m_shotsThisFrame;
                    m_fireCooldownRemaining = weaponStat.fireIntervalSeconds;
                }
            }
        }

        if (m_shotsThisFrame > 0)
        {
            m_weaponAnimationFrame = 1;
            m_weaponAnimationTime = 0.0f;
        }

        if (m_weaponAnimationFrame == 0)
        {
            return;
        }

        m_weaponAnimationTime += deltaTime;

        const float weaponAnimationTick = weaponStat.fireIntervalSeconds / 4.0f;

        while (m_weaponAnimationTime >= weaponAnimationTick)
        {
            m_weaponAnimationTime -= weaponAnimationTick;

            if (m_weaponAnimationFrame >= LastWeaponAnimationFrame)
            {
                m_weaponAnimationFrame = 0;
                m_weaponAnimationTime = 0.0f;
                break;
            }

            ++m_weaponAnimationFrame;
        }
    }

    void WolfPlayer::takeDamage(int32_t damage)
    {
        if (isAlive() == false || damage <= 0)
        {
            return;
        }
        if (GodMode == false)
        {
            m_health = std::max(0, m_health - damage);
        }
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

    void WolfPlayer::setAmmo(int32_t amount)
    {
        m_ammo = std::clamp(amount, 0, MaxAmmo);
    }

    bool WolfPlayer::consumeAmmo()
    {
        if (UnlimitedAmmo == true)
        {
            return true;
        }
        if (m_ammo <= 0)
        {
            return false;
        }
        --m_ammo;
        return true;
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
