#pragma once

#include <cstdint>
#include <vector>
#include <array>

namespace wc
{

    struct WolfVgaHuffmanNode
    {
        uint16_t bit0 = 0;
        uint16_t bit1 = 0;
    };

    struct WolfVgaGraphicsData
    {
        std::vector<uint32_t> chunkOffsets;
        std::array<WolfVgaHuffmanNode, 255> huffmanNodes;
        std::vector<uint8_t> graphBytes;
    };

    struct WolfVgaPictureDimension
    {
        uint16_t width;
        uint16_t height;
    };

}