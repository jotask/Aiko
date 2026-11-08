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

        AIKO_ASSERT(data.size() >= 6, "VSWAP header is incomplete");

        const uint16_t totalChunkCount = readU16LE(data, 0);
        AIKO_ASSERT(size_t(totalChunkCount) * 6 <= data.size() - 6, "VSWAP chunk directory exceeds file bounds");
        const uint16_t firstSpriteChunkIndex = readU16LE(data, 2);
        const uint16_t firstSoundChunkIndex = readU16LE(data, 4);

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

        std::vector<WolfSprite> sprites;
        sprites.resize(firstSoundChunkIndex - firstSpriteChunkIndex);
        const uint16_t endOfSprites = firstSoundChunkIndex;
        for (size_t spriteIndex = firstSpriteChunkIndex ; spriteIndex < endOfSprites; ++spriteIndex)
        {
            const uint32_t chunkOffset = chunksOffset[spriteIndex];
            const uint32_t chunkLength = chunksLength[spriteIndex];
            if (chunkOffset == 0 && chunkLength == 0)
            {
                continue;
            }

            AIKO_ASSERT(chunkOffset <= data.size(), "Sprite chunk offset out of bounds");
            AIKO_ASSERT(chunkLength <= data.size() - chunkOffset, "Sprite chunk exceeds file bounds");
            AIKO_ASSERT(chunkLength >= 4, "Sprite chunk too small for header");

            const uint16_t firstCol = readU16LE(data, chunkOffset + 0);
            const uint16_t lastCol = readU16LE(data, chunkOffset + 2);

            AIKO_ASSERT(firstCol <= lastCol, "Invalid sprite column range");
            AIKO_ASSERT(lastCol < 64, "Sprite column exceeds texture width");
            AIKO_ASSERT(4 + 2 * (lastCol - firstCol + 1) <= chunkLength, "Sprite column offset table exceeds chunk bounds");

            const size_t chunkEnd = size_t(chunkOffset) + chunkLength;

            for (size_t column = firstCol; column <= lastCol ; ++column)
            {
                const uint16_t columnOffset = readU16LE(data, chunkOffset + 4 + 2 * ( column - firstCol ));
                size_t commandOffset = chunkOffset + columnOffset;

                AIKO_ASSERT(columnOffset <= chunkLength && chunkLength - columnOffset >= 2, "Sprite column command offset out of bounds");

                while (true)
                {
                    AIKO_ASSERT(commandOffset <= chunkEnd && chunkEnd - commandOffset >= 2, "Sprite post terminator exceeds chunk bounds");
                    const uint16_t endRowTimesTwo = readU16LE(data, commandOffset + 0);
                    if (endRowTimesTwo == 0)
                    {
                        break;
                    }

                    AIKO_ASSERT(chunkEnd - commandOffset >= 6, "Sprite post command exceeds chunk bounds");

                    const int16_t pixelBaseOffset = static_cast<int16_t>(readU16LE(data, commandOffset + 2));
                    const uint16_t startRowTimesTwo = readU16LE(data, commandOffset + 4);

                    const uint16_t startRow = startRowTimesTwo / 2;
                    const uint16_t endRow = endRowTimesTwo / 2;

                    AIKO_ASSERT((startRowTimesTwo % 2) == 0 && (endRowTimesTwo % 2) == 0, "Sprite post row coordinates must be even");
                    AIKO_ASSERT(startRow < endRow && endRow <= 64, "Invalid sprite post row range");
                    const int32_t pixelStart = int32_t(pixelBaseOffset) + startRow;
                    const int32_t pixelEnd = int32_t(pixelBaseOffset) + endRow;
                    AIKO_ASSERT(pixelStart >= 0 && pixelEnd <= int32_t(chunkLength), "Sprite post pixel data exceeds chunk bounds");

                    for (size_t row = startRow; row < endRow ; ++row)
                    {
                        const size_t pixelOffset = size_t(int32_t(pixelBaseOffset) + int32_t(row));
                        const uint8_t paletteIndex = data[size_t(chunkOffset) + pixelOffset];
                        sprites[spriteIndex - firstSpriteChunkIndex].paletteIndices[row * 64 + column] = paletteIndex;
                        sprites[spriteIndex - firstSpriteChunkIndex].alphaMask[row * 64 + column] = 255;
                    }

                    commandOffset += 6;

                }

            }
        }

        const WolfVSwapData result = {
            .walls = std::move(walls),
            .sprites = std::move(sprites),
        };

        return result;

    }
}
