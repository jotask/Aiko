#include "wolf_enemy.h"

namespace wc
{
    WolfEnemy::WolfEnemy(aiko::vec2 position, WolfEnemyType type, WolfDirection direction, WolfEnemySpawnMode mode)
        : m_position(position)
        , m_type(type)
        , m_direction(direction)
        , m_spawnMode(mode)
    {

    }

    WolfEnemy WolfEnemy::fromSpawn(const WolfEnemySpawn& spawn)
    {
        aiko::vec2 position = aiko::vec2(spawn.tile.x + 0.5f, spawn.tile.y + 0.5f );
        return WolfEnemy(position, spawn.type, spawn.direction, spawn.spawnMode);
    }
}
