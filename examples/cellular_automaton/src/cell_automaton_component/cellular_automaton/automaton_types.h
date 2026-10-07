#pragma once

#include "aiko_types.h"
#include "types/color.h"

#include <execution>

namespace aiko::ca
{

    constexpr bool DEBUG_CHUNKS = false;
    constexpr bool DRAW_DEAD_CELLS = true;
    constexpr bool RANDOM_CELL_INIT = true;

    inline constexpr auto ExecutionPolicy = std::execution::seq;

    constexpr ivec2 SIZE_WORLD = { 6, 4 }; // How many initial chunks
    constexpr ivec2 SIZE_CHUNK = { 16 }; // How many cells in a chunk

    constexpr bool WORLD_FPS_TIMER_LOCK = true;
    constexpr float WORLD_FRAME_RATE = 30.0f;

    constexpr ivec2 NEIGHBOURS = { 1,  1 };

}