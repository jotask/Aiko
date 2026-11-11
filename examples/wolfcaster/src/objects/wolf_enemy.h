#pragma once

#include "map/wolf_map_types.h"

#include <optional>

namespace wc
{
    class WolfMap;
    class WolfEnemy
    {
    public:

        static constexpr float EnemyRadius = 0.2f;
        static constexpr float AttackRangeEnemy = 5.0f;
        static constexpr float AttackRangeDog = 0.8f;

        void update(float deltaTime, WolfMap& map, const aiko::vec2& playerPosition);

        void takeDamage(int32_t damage);

        const aiko::vec2& position() const { return m_position; }
        const WolfEnemyType type() const { return m_type; }
        const WolfDirection direction() const { return m_direction; }
        const WolfEnemySpawnMode spawnMode() const { return m_spawnMode; }

        const WolfEnemyAnimation animation() const { return m_animation; }
        const uint8_t animationFrame() const { return m_animationFrame; }

        const WolfEnemyState state() const { return m_state; }

        bool shotThisFrame() const { return m_shotPending; }

        int32_t health() const { return m_health; }
        bool isAlive() const { return health() > 0; }

        static WolfEnemy fromSpawn(const WolfEnemySpawn&);

    private:

        WolfEnemy() = delete;
        explicit WolfEnemy(aiko::vec2 position, WolfEnemyType type, WolfDirection direction, WolfEnemySpawnMode mode);

        void transitionTo(WolfEnemyState state);

        void updateStanding(float deltaTime, WolfMap& map, const aiko::vec2& playerPosition);
        void updatePatrolling(float deltaTime, WolfMap& map, const aiko::vec2& playerPosition);
        void updateAttacking(float deltaTime, WolfMap& map, const aiko::vec2& playerPosition);
        void updateChasing(float deltaTime, WolfMap& map, const aiko::vec2& playerPosition);

        bool tryDetectPlayer(const WolfMap& map, const aiko::vec2& playerPosition);
        bool canSeePlayer(const WolfMap& map, const aiko::vec2& playerPosition) const;
        bool canAttackPlayer(const WolfMap& map, const aiko::vec2& playerPosition) const;

        uint8_t getAttackFrameCount() const;
        uint8_t getDeathFrameCount() const;

        aiko::vec2 m_position = {};
        WolfEnemyType m_type = WolfEnemyType::Guard;
        WolfDirection m_direction = WolfDirection::North;
        WolfEnemySpawnMode m_spawnMode = WolfEnemySpawnMode::Standing;

        // Animation
        WolfEnemyAnimation m_animation = WolfEnemyAnimation::Idle;
        uint8_t m_animationFrame = 0;
        float m_animationTime = 0.0f;

        // Machine state
        WolfEnemyState m_state = WolfEnemyState::Standing;
        std::optional<aiko::vec2> m_patrolTargetPosition;

        // Attacking
        float m_attackCooldownRemaining = 0.0f;
        bool m_shotPending = false;

        // Health
        static constexpr int32_t MaxHealth = 100;
        int32_t m_health = MaxHealth;

    };
}
