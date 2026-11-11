#include "../map/carmack_decoder.h"

#include "byte_utils.h"
#include "logger/logger.h"

namespace wc
{

    std::vector<uint16_t> CarmackDecoder::decode(std::span<const uint8_t> data)
    {
        const uint16_t expandedSizeBytes = readU16LE(data, 0);
        AIKO_ASSERT(expandedSizeBytes > 0, "Expanded size byte to small?");
        AIKO_ASSERT(expandedSizeBytes % 2 == 0, "Expanded not valid");

        const size_t expectedWordCount = expandedSizeBytes / 2;

        aiko::logger::Log::info("CarmackDecoder");
        aiko::logger::Log::infoFmt("     Expanded size Byte: {} Expected Word Count: {}", expandedSizeBytes, expectedWordCount);

        std::vector<uint16_t> decompressedWords;
        decompressedWords.reserve(expectedWordCount);

        size_t inputOffset = 2;
        while (expectedWordCount > decompressedWords.size())
        {
            const uint16_t word = readU16LE(data, inputOffset);
            inputOffset += 2;
            const uint8_t highByte = getHighByte(word);
            if (highByte != 0xA7 && highByte != 0xA8)
            {
                decompressedWords.push_back(word);
                continue;
            }
            // Near reference
            if (highByte == 0xA7)
            {
                const uint8_t count = getLowByte(word);
                AIKO_ASSERT(count <= expectedWordCount - decompressedWords.size(), "Producing words than expected");
                if (count > 0)
                {
                    AIKO_ASSERT(inputOffset < data.size(), "Nothing more to read?");
                    uint8_t distance = data[inputOffset++];
                    AIKO_ASSERT(distance > 0 && distance <= decompressedWords.size(), "Invalid backward reference");
                    const size_t startIndex = decompressedWords.size() - distance;
                    for (size_t i = 0 ; i < count ; ++i)
                    {
                        decompressedWords.push_back(decompressedWords[startIndex + i]);
                    }
                }
                else
                {
                    AIKO_ASSERT(inputOffset < data.size(), "Out Of Bounds!");
                    const uint8_t escape = data[inputOffset];
                    inputOffset++;
                    const uint16_t literal = wordByte(highByte, escape);
                    decompressedWords.push_back(literal);
                }
                continue;
            }

            // Far byte
            if (highByte == 0xA8)
            {
                const uint8_t count = getLowByte(word);
                AIKO_ASSERT(count <= expectedWordCount - decompressedWords.size(), "Producing words than expected");
                if (count > 0)
                {
                    uint16_t absoluteWordOffset = readU16LE(data, inputOffset);
                    AIKO_ASSERT(absoluteWordOffset < decompressedWords.size(), "Absolute world Offset out Of Bounds");
                    inputOffset += 2;
                    const size_t startIndex = absoluteWordOffset;
                    for (size_t i = 0 ; i < count ; ++i)
                    {
                        decompressedWords.push_back(decompressedWords[startIndex + i]);
                    }
                }
                else
                {
                    AIKO_ASSERT(inputOffset < data.size(), "Out Of Bounds!");
                    const uint8_t escape = data[inputOffset];
                    inputOffset++;
                    const uint16_t literal = wordByte(highByte, escape);
                    decompressedWords.push_back(literal);
                }
                continue;
            }

        }

        AIKO_ASSERT(decompressedWords.size() == expectedWordCount, "We have not decompressed expected Words");

        aiko::logger::Log::infoFmt("     Successfully Decoded -> Decompressed {} words : Input Offset {} : DecompressedWords {} (0x{:08X})", decompressedWords.size(), inputOffset, decompressedWords[0], decompressedWords[0]);

        return decompressedWords;
    }

}
