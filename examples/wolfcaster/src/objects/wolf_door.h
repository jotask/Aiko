#pragma once

#include "map/wolf_map_types.h"
#include "math/math_vector.h"

#include <cstdint>

namespace wc
{

    enum class WolfDoorOrientation
    {
        Horizontal,
        Vertical,
    };

    enum class WolfDoorKind
    {
        Normal,
        Locked,
        Elevator,
        PushWall,
    };

    enum class WolfDoorState
    {
        Closed,
        Opening,
        Open,
        Closing
    };

    struct WolfDoor
    {
        aiko::vec2def<uint16_t> tileCoord = {0};
        WolfDoorOrientation orientation = WolfDoorOrientation::Horizontal;
        WolfDoorKind kind = WolfDoorKind::Normal;
        WolfDoorState state = WolfDoorState::Closed;

        uint8_t requiredKeyIndex = 0;

        float openProgress = 0.0f;
        float timer = 0.f;

        // Push-wall only
        aiko::ivec2 pushDirection = {0};
        TileId wallTileId = 0;

    };

}
