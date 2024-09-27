#pragma once

#include "aiko_types.h"
#include "types/color.h"

namespace aiko::ca
{

    namespace cellautomaton
    {

        constexpr bool DEBUG_CHUNKS = false;
        constexpr const uint DRAW_DEAD_CELLS = false;
        constexpr bool RANDOM_CELL_INIT = true;

        constexpr bool ASYNC_UPDATE_CHUNK = false;
        constexpr bool ASYNC_UPDATE_CELL = false;

        constexpr const uint SIZE_WORLD = 32; // How many initial chunks
        constexpr const uint SIZE_CHUNK = 4; // How many cells in a chunk

        constexpr bool WORLD_FPS_TIMER_LOCK = true;
        constexpr float WORLD_FRAME_RATE = 1.0f;

        const ivec2 NEIGHBOURS = { 1,  1 };

        inline int getIndex(int x, int y, int width)
        {
            return width * x + y;
        }

        inline int getChunkIndex(int x, int y, int width)
        {
            return y * width + x;
        }

    }

}