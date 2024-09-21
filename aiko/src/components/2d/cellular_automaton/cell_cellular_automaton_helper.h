#pragma once

#include "aiko_types.h"
#include "types/color.h"

namespace aiko
{

    namespace cellautomaton
    {

        constexpr const uint SIZE_WORLD = 4; // How many initial chunks
        constexpr const uint SIZE_CHUNK = 32; // How many cells in a chunk

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