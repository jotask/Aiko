#include "wolf_vswap_reader.h"

#include "core/file.h"
#include "map/byte_utils.h"

#include <vector>
#include <utility>

namespace wc
{

    WolfVSwapData WolfVSwapReader::load(const std::filesystem::path& path)
    {
        AIKO_ASSERT(std::filesystem::exists(path), "File dont' exist");

        const std::vector<uint8_t> data = aiko::files::readFileBytes(path);

        uint16_t totalChunkCount = readU16LE(data, 0);

        uint16_t firstSpriteChunkIndex = readU16LE(data, 2);
        uint16_t firstSoundChunkIndex = readU16LE(data, 4);

        AIKO_ASSERT(firstSpriteChunkIndex <= firstSoundChunkIndex, "Expected sizes dont match!");
        AIKO_ASSERT(firstSoundChunkIndex <= totalChunkCount, "Expected sizes dont match!");

        const auto directorySize = 6 + totalChunkCount * 6;

        std::vector<uint32_t> chunksOffset;
        for (size_t i = 0 ; i < totalChunkCount; ++i)
        {
            const uint32_t word = readU32LE(data ,6 + i * 4);
            chunksOffset.push_back(word);
        }

        std::vector<uint16_t> chunksLength;
        for (size_t i = 0 ; i < totalChunkCount; ++i)
        {
            const uint16_t word = readU16LE(data ,6 + 4 * totalChunkCount + i * 2);
            chunksLength.push_back(word);
        }

        const uint16_t endOfWallTexture = firstSpriteChunkIndex;

        std::vector<WolfWallTexture> walls;
        walls.resize(endOfWallTexture);
        for (size_t wallIndex = 0 ; wallIndex < endOfWallTexture; ++wallIndex)
        {
            const uint32_t chunkOffset = chunksOffset[wallIndex];
            const uint32_t chunkLength = chunksLength[wallIndex];
            if (chunkOffset == 0 && chunkLength == 0)
            {
                continue;
            }
            AIKO_ASSERT(chunkOffset <= data.size(), "Chunk Offset out of range");
            AIKO_ASSERT(chunkLength <= data.size() - chunkOffset, "Chunk size out of range");
            AIKO_ASSERT(chunkLength == 64 * 64, "Chunk size not expected");

            for (size_t y = 0 ; y < 64 ; ++y)
            {
                for (size_t x = 0 ; x < 64 ; ++x)
                {
                    const auto sourceIndex = x  * 64 + y;
                    const auto destinationIndex = y * 64 + x;

                    const uint8_t pixel = data[chunkOffset + sourceIndex];
                    walls[wallIndex].paletteIndices[destinationIndex] = pixel;
                }
            }

        }

        const WolfVSwapData result = {
            .walls = std::move(walls),
        };

        return result;

    }
}
