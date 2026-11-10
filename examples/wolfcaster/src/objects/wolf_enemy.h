#pragma once

#include "map/wolf_map_types.h"

namespace wc
{
    class WolfEnemy
    {
    public:

        void update(float deltaTime);

        const aiko::vec2& position() const { return m_position; }
        const WolfEnemyType type() const { return m_type; }
        const WolfDirection direction() const { return m_direction; }
        const WolfEnemySpawnMode spawnMode() const { return m_spawnMode; }
        const WolfEnemyAnimation animation() const { return m_animation; }
        const uint8_t animationFrame() const { return m_animationFrame; }

        static WolfEnemy fromSpawn(const WolfEnemySpawn&);

    private:
        WolfEnemy() = delete;
        explicit WolfEnemy(aiko::vec2 position, WolfEnemyType type, WolfDirection direction, WolfEnemySpawnMode mode);
        aiko::vec2 m_position = {};
        WolfEnemyType m_type = WolfEnemyType::Guard;
        WolfDirection m_direction = WolfDirection::North;
        WolfEnemySpawnMode m_spawnMode = WolfEnemySpawnMode::Standing;

        WolfEnemyAnimation m_animation = WolfEnemyAnimation::Idle;
        uint8_t m_animationFrame = 0;
        float m_animationTime = 0.0f;
    };
}
