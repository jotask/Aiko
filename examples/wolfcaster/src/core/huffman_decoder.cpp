#include "huffman_decoder.h"

#include "aiko_macros.h"
#include "byte_utils.h"
#include "logger/logger.h"

namespace wc
{
    std::vector<std::uint8_t> HuffmanDecoder::decode(std::span<const uint8_t> compressedBytes, const std::array<WolfVgaHuffmanNode, 255>& dictionary, size_t expectedDecompressionLength)
    {

        AIKO_ASSERT(compressedBytes.empty() == false, "Huffman compressed input is empty");
        AIKO_ASSERT(expectedDecompressionLength > 0, "Invalid Huffman expected output size");

        std::vector<uint8_t> output;
        output.reserve(expectedDecompressionLength);

        size_t byteIndex = 0;
        uint8_t bitIndex = 0;
        size_t nodeIndex = 254;

        while (output.size() < expectedDecompressionLength)
        {

            AIKO_ASSERT(byteIndex < compressedBytes.size(), "Huffman input exhausted");
            AIKO_ASSERT(bitIndex < 8, "Invalid Huffman bit index");
            AIKO_ASSERT(nodeIndex < dictionary.size(), "Huffman node index out of bounds");

            const uint8_t compressedByte = compressedBytes[byteIndex];
            const uint8_t bit = getBit(compressedByte, bitIndex);

            ++bitIndex;

            if (bitIndex == 8)
            {
                bitIndex = 0;
                ++byteIndex;
            }

            uint16_t branchValue = 0;
            if (bit == 0)
            {
                branchValue = dictionary[nodeIndex].bit0;
            }
            else if (bit == 1)
            {
                branchValue = dictionary[nodeIndex].bit1;
            }
            AIKO_ASSERT(branchValue <= 510, "Invalid Huffman branch reference");

            if (branchValue < 256)
            {
                output.push_back(static_cast<uint8_t>(branchValue));
                nodeIndex = 254;
                continue;
            }

            nodeIndex = branchValue - 256;

        }

        AIKO_ASSERT(output.size() == expectedDecompressionLength, "Huffman output size mismatch");

        aiko::logger::Log::infoFmt("Huffman decoded: compressed={} bytes, decompressed={} bytes", compressedBytes.size(), output.size());
        aiko::logger::Log::infoFmt("Huffman first byte: 0x{:02X}", output.front());

        return output;
    }
}
