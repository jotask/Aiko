#pragma once

namespace wc
{
    class WolfMap;
    class WolfPlayer;
    class WolfCombatSystem final
    {
    public:
        static void update(WolfMap& map, WolfPlayer& player);
    private:
        static void resolvePlayerShot(WolfMap& map, WolfPlayer& player);
        static void resolveEnemyShots(WolfMap& map, WolfPlayer& player);
    };
}
