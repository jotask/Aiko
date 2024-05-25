#pragma once

#include <string>
#include <stdint.h>

namespace nes
{

    using Byte = uint8_t;
    using Word = uint16_t;

    using u32 = uint32_t;
    using u64 = uint64_t;

    constexpr u32 NES_WIDTH = 256;
    constexpr u32 NES_HEIGHT = 240;

    constexpr const Byte COLOUR_PALETTE_SIZE = 0x40;

}
