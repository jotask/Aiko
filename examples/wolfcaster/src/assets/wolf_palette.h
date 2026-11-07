#pragma once

#include "types/color.h"

#include <cstdint>
#include <array>

namespace wc
{
    class WolfPalette
    {
    public:
        static aiko::Color getColor(uint8_t paletteIndex);
    private:
        static const std::array<aiko::Color, 256> paletteColors;
    };
}
