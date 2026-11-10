#pragma once

#include <math/math_vector.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace wc
{

    using TileId = uint16_t;
    using Tile = aiko::vec2def<uint16_t>;

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
        Tile tile = {0};
        WolfDirection direction = WolfDirection::North;
    };

    struct WolfLevelInfo
    {
        uint16_t index = 0;
        std::string name;
        uint16_t width = 0;
        uint16_t height = 0;
    };

    struct WolfWorldObject
    {
        aiko::vec2 position = {0.0f};
        TileId mapObjectId = 0;
        size_t spriteIndex = 0;
        bool blocking = false;
    };

    struct WolfMapData
    {
        WolfLevelInfo level;

        std::vector<TileId> geometry;
        std::vector<TileId> info;

        WolfPlayerSpawn playerSpawn;
        std::vector<WolfWorldObject> objects;
    };

}