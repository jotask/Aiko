#pragma once

#include "map/wolf_map_types.h"

namespace wc
{
    class WolfEnemy
    {
    public:

        const aiko::vec2& position() const { return m_position; }
        const WolfEnemyType type() const { return m_type; }
        const WolfDirection direction() const { return m_direction; }
        const WolfEnemySpawnMode spawnMode() const { return m_spawnMode; }

        static WolfEnemy fromSpawn(const WolfEnemySpawn&);

    private:
        WolfEnemy() = delete;
        explicit WolfEnemy(aiko::vec2 position, WolfEnemyType type, WolfDirection direction, WolfEnemySpawnMode mode);
        aiko::vec2 m_position;
        WolfEnemyType m_type;
        WolfDirection m_direction;
        WolfEnemySpawnMode m_spawnMode;
    };
}
