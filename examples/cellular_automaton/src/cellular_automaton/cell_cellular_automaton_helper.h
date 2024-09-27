#pragma once

#include "aiko_types.h"
#include "types/color.h"

namespace aiko::ca
{

    namespace cellautomaton
    {

        constexpr bool DEBUG_CHUNKS = true;
        constexpr const uint DRAW_DEAD_CELLS = true;
        constexpr bool RANDOM_CELL_INIT = true;

        constexpr const uint SIZE_WORLD = 2; // How many initial chunks
        constexpr const uint SIZE_CHUNK = 32; // How many cells in a chunk

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