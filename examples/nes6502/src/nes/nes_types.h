#pragma once

#include <string>
#include <stdint.h>

namespace nes
{

    using Byte = uint8_t;
    using Word = uint16_t;

    using u32 = uint32_t;
    using u64 = uint64_t;

    constexpr Byte NES_WIDTH = 256;
    constexpr Byte NES_HEIGHT = 240;

}
