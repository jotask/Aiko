#pragma once

#include "assets/wolf_vga_graphics_types.h"

#include <cstdint>
#include <vector>
#include <span>

namespace wc
{
    class HuffmanDecoder final
    {
    public:
        static std::vector<std::uint8_t> decode(const std::span<const uint8_t> compressedBytes, const std::array<WolfVgaHuffmanNode, 255>& dictionary, size_t expectedDecompressionLength);
    };
}
