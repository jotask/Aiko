#include "wolf_enemy.h"

namespace wc
{
    WolfEnemy::WolfEnemy(aiko::vec2 position, WolfEnemyType type, WolfDirection direction, WolfEnemySpawnMode mode)
        : m_position(position)
        , m_type(type)
        , m_direction(direction)
        , m_spawnMode(mode)
        , m_animation(mode == WolfEnemySpawnMode::Patrolling ? WolfEnemyAnimation::Walk : WolfEnemyAnimation::Idle)
    {
    }

    void WolfEnemy::update(float deltaTime)
    {
        constexpr float AnimationTick = 0.15f;
        if (m_animation == WolfEnemyAnimation::Idle)
        {
            m_animationFrame = 0;
            return;
        }
        m_animationTime += deltaTime;
        if (m_animationTime >= AnimationTick)
        {
            ++m_animationFrame;
            m_animationFrame %= 4;
            m_animationTime -= AnimationTick;
        }
    }

    WolfEnemy WolfEnemy::fromSpawn(const WolfEnemySpawn& spawn)
    {
        aiko::vec2 position = aiko::vec2(spawn.tile.x + 0.5f, spawn.tile.y + 0.5f );
        return WolfEnemy(position, spawn.type, spawn.direction, spawn.spawnMode);
    }
}
