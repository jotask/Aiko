#pragma once

#include "objects/wolf_enemy.h"

#include <vector>

namespace wc
{
    class WolfMap;
    class WolfEnemySystem
    {
    public:
        void initialize(const WolfMap& map);
        void update(float deltaTime, WolfMap& map);

        const std::vector<WolfEnemy>& getEnemies() const { return m_enemies; }

    private:
        std::vector<WolfEnemy> m_enemies;
    };

}
