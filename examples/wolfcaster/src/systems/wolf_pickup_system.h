#pragma once

namespace wc
{
    class WolfMap;
    class WolfPlayer;
    class WolfPickupSystem final
    {
    public:
        static void updateMapPickups(WolfMap& map, WolfPlayer& player);
    };
}
