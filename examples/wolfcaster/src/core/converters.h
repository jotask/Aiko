#pragma once

#include "map/wolf_map_types.h"
#include "math/math.h"

namespace wc
{

    static float convertDirectionToAngle(const WolfDirection dir)
    {
        switch (dir)
        {
            case WolfDirection::East:   return static_cast<float>(0.0);
            case WolfDirection::NorthEast:  return static_cast<float>(aiko::PI / 4.0);
            case WolfDirection::North:  return static_cast<float>(aiko::PI / 2.0);
            case WolfDirection::NorthWest:  return static_cast<float>((3.0 * aiko::PI) / 4.0);
            case WolfDirection::West:   return static_cast<float>(aiko::PI);
            case WolfDirection::SouthWest:  return static_cast<float>((5.0 * aiko::PI) / 4.0);
            case WolfDirection::South:  return static_cast<float>((3.0 * aiko::PI) / 2.0);
            case WolfDirection::SouthEast:  return static_cast<float>((7.0 * aiko::PI) / 4.0);
        }
        AIKO_ASSERT(false, "Unknown direction");
        return 0;
    };

    static aiko::vec2 convertDirectionToVector(const WolfDirection dir)
    {
        switch (dir)
        {
            case WolfDirection::North:      return {0, -1};
            case WolfDirection::NorthEast:  return {1, -1};
            case WolfDirection::East:       return {1, 0};
            case WolfDirection::SouthEast:  return {1, 1};
            case WolfDirection::South:      return {0, 1};
            case WolfDirection::SouthWest:  return {-1, 1};
            case WolfDirection::West:       return {-1, 0};
            case WolfDirection::NorthWest:  return {-1, -1};
        }
        AIKO_ASSERT(false, "Unknown direction");
        return {0};
    };

}