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

        void updateWeapon(float deltaTime, bool triggerPressed);

        void takeDamage(int32_t damage);

        void move(const aiko::vec2 displacement, const WolfMap& map);
        void rotate(float angle);

        const aiko::vec2& position() const { return m_position; }
        float angle() const { return m_angleRadians; }

        aiko::vec2 forward() const;
        aiko::vec2 right() const;

        int32_t health() const { return m_health; }
        bool isAlive() const { return health() > 0; }

        bool shotThisFrame() const { return m_shotThisFrame; }

        static WolfPlayer fromSpawn(const WolfPlayerSpawn& spawn);

    private:

        aiko::vec2 m_position = {0.0f};
        float m_angleRadians = 0.0f;

        // Shooting
        float m_fireCooldownRemaining = 0.0f;
        bool m_shotThisFrame = false;

        // Health
        static constexpr int32_t MaxHealth = 100;
        int32_t m_health = MaxHealth;

    };

}

