#include "wolf_enemy_system.h"

#include "wolf_map.h"

namespace wc
{

    void WolfEnemySystem::initialize(const WolfMap& map)
    {
        m_enemies.clear();
        constexpr WolfDifficulty currentSelectedDifficulty = WolfDifficulty::Hard; // Let's add all enemies
        for (const WolfEnemySpawn& spawn : map.getEnemySpawns())
        {
            if (currentSelectedDifficulty < spawn.minDifficulty)
            {
                continue;
            }
            const WolfEnemy enemy = WolfEnemy::fromSpawn(spawn);
            m_enemies.push_back(enemy);
        }
    }

    void WolfEnemySystem::update(float deltaTime)
    {
        for (WolfEnemy& enemy : m_enemies)
        {
            enemy.update(deltaTime);
        }
    }

}
