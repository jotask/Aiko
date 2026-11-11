#pragma once

#include "map/wolf_map_types.h"

namespace wc
{
    struct WolfPushWall
    {
        Tile tile = {0};
        aiko::ivec2 direction = {0};
        TileId wallTileId = 0;
        float distance = 0.0f;
    };
}