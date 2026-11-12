#pragma once

#include "map/wolf_map_types.h"
#include "math/math_vector.h"

namespace wc
{
    class WolfMap;

    class WolfPlayer
    {
    public:

        static constexpr float PlayerRadius = 0.2f;
        static constexpr int32_t MaxHealth = 100;
        static constexpr int32_t MaxAmmo = 99;
        static constexpr int32_t StartingAmmo = 8;

        void updateWeapon(float deltaTime, bool triggerHeld, bool triggerJustPressed);

        void takeDamage(int32_t damage);

        void move(const aiko::vec2 displacement, const WolfMap& map);
        void rotate(float angle);

        const aiko::vec2& position() const { return m_position; }
        float angle() const { return m_angleRadians; }

        void setWeapon(WolfWeapon weapon);
        WolfWeapon getWeapon() const { return m_currentWeapon; };

        aiko::vec2 forward() const;
        aiko::vec2 right() const;

        int32_t health() const { return m_health; }
        bool heal(int32_t amount);
        bool isAlive() const { return health() > 0; }

        int32_t ammo() const { return m_ammo; }
        bool addAmmo(int32_t amount);
        void setAmmo(int32_t amount);
        bool consumeAmmo();

        uint32_t score() const { return m_score; }
        void addScore(uint32_t amount) { m_score += amount; }

        uint32_t shotsThisFrame() const { return m_shotsThisFrame; }

        uint8_t weaponAnimationFrame() const { return m_weaponAnimationFrame; }

        static WolfPlayer fromSpawn(const WolfPlayerSpawn& spawn);

    private:

        aiko::vec2 m_position = {0.0f};
        float m_angleRadians = 0.0f;

        // Shooting
        float m_fireCooldownRemaining = 0.0f;
        uint32_t m_shotsThisFrame = 0;

        // Health
        int32_t m_health = MaxHealth;

        // Animation
        uint8_t m_weaponAnimationFrame = 0;
        float m_weaponAnimationTime = 0.0f;

        // Weapon
        WolfWeapon m_currentWeapon = WolfWeapon::Pistol;
        int32_t m_ammo = StartingAmmo;

        // Score
        uint32_t m_score = 0;

    };

}

