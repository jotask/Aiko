#pragma once

#include <cstdint>

namespace wc
{
    enum class WolfMapPlane : uint8_t
    {
        Geometry = 0,
        Objects = 1,
        Info = 2,
    };

    enum class WolfDirection : uint8_t
    {
        North = 19,
        East = 20,
        South = 21,
        West = 22,
    };

    struct WolfPlayerSpawn
    {
        uint16_t tileX;
        uint16_t tileY;
        WolfDirection direction;
    };

}