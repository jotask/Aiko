#pragma once

#include <string>
#include <stdint.h>

namespace nes
{
#if true
    const std::string AssetPath = "C:/Users/j.iznardo/Documents/Aiko/examples/nes6502/assets/";
#else
    const std::string AssetPath = "C:/Users/Jose/Documents/Aiko/examples/nes6502/assets/";
#endif

    using Byte = uint8_t;
    using Word = uint16_t;

    using u32 = uint32_t;
    using u64 = uint64_t;

}
