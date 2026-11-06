#include "rlew_decoder.h"

#include "byte_utils.h"
#include "logger/logger.h"

namespace wc
{

    std::vector<uint16_t> RlewDecoder::decode(std::span<const uint16_t> data, uint16_t rlewFlag)
    {
        AIKO_ASSERT(data.empty() == false, "Not data to decode?");
        const uint16_t expandedSizeBytes = data[0];
        AIKO_ASSERT(expandedSizeBytes > 0, "Expanded size byte to small?");
        AIKO_ASSERT(expandedSizeBytes % 2 == 0, "Expanded not valid");

        const size_t expectedWordCount = expandedSizeBytes / 2;

        std::vector<uint16_t> decompressedWords;
        decompressedWords.reserve(expectedWordCount);

        size_t inputOffset = 1;
        while (expectedWordCount > decompressedWords.size())
        {
            AIKO_ASSERT(inputOffset < data.size(), "Out Of Bounds!");
            const uint16_t word = data[inputOffset++];
            if (word != rlewFlag)
            {
                decompressedWords.push_back(word);
            }
            else
            {
                AIKO_ASSERT(inputOffset <= data.size() && data.size() - inputOffset >= 2, "Not enough input words for RLEW sequence");
                const uint16_t count = data[inputOffset++];
                AIKO_ASSERT(count <= expectedWordCount - decompressedWords.size(), "RLEW run exceeds expected output size");
                const uint16_t wordToRepeat = data[inputOffset++];
                for (size_t i = 0 ; i < count ; ++i)
                {
                    decompressedWords.push_back(wordToRepeat);
                }
            }
        }

        AIKO_ASSERT(decompressedWords.size() == expectedWordCount, "We have not decompressed expected Words");

        aiko::logger::Log::info("RlewDecoder");
        aiko::logger::Log::infoFmt("     Input: {} words. Output: {} words. Words Consumed: {}", data.size(), decompressedWords.size(), inputOffset);

        return decompressedWords;

    }
}
