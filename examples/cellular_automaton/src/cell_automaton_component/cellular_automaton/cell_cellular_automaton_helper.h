#pragma once

#include "automaton_types.h"

namespace aiko::ca
{

    inline int getIndex(int x, int y, int width)
    {
        return y * width + x;
    }

    inline int getChunkIndex(int x, int y, int width)
    {
        return y * width + x;
    }

}