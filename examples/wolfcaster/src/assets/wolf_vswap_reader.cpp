#include "wolf_vswap_reader.h"

#include "../core/byte_utils.h"
#include "core/file.h"

#include <utility>
#include <vector>

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

        // Wall Textures
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

        // Sprites
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

        // Sounds
        std::vector<WolfDigitizedSound> sounds;
        AIKO_ASSERT(firstSoundChunkIndex < totalChunkCount, "VSWAP sound information chunk is missing");
        const size_t soundInfoChunkIndex = totalChunkCount - 1;
        const uint32_t soundInfoOffset = chunksOffset[soundInfoChunkIndex];
        const uint32_t soundInfoLength = chunksLength[soundInfoChunkIndex];
        const uint32_t soundPageCount = soundInfoChunkIndex - firstSoundChunkIndex;
        AIKO_ASSERT(soundInfoOffset <= data.size(), "Sound information offset out of bounds");
        AIKO_ASSERT(soundInfoLength <= data.size() - soundInfoOffset, "Sound information exceeds file bounds");
        AIKO_ASSERT(soundInfoLength >= 4, "Sound information chunk is too small");

        const size_t soundCount = soundInfoLength / 4;
        sounds.resize(soundCount);
        for (size_t soundIndex  = 0 ; soundIndex < soundCount ; ++soundIndex)
        {
            const uint16_t startPage = readU16LE(data, soundInfoOffset + soundIndex * 4);
            const uint16_t soundLengthLow = readU16LE(data, soundInfoOffset + soundIndex * 4 + 2);
            if (startPage >= soundPageCount)
            {
                break;
            }
            if (soundIndex > 0 && startPage == 0)
            {
                break;
            }
            const size_t firstPageIndex = firstSoundChunkIndex + startPage;

            size_t lastPageIndex = soundInfoChunkIndex;
            if (soundIndex + 1 < soundCount)
            {
                const uint16_t nextStartPage = readU16LE(data, soundInfoOffset + (soundIndex + 1) * 4);
                if (nextStartPage > startPage && nextStartPage < soundPageCount)
                {
                    lastPageIndex = firstSoundChunkIndex + nextStartPage;
                }
                else
                {
                    AIKO_ASSERT(nextStartPage == 0 || nextStartPage >= soundPageCount, "Invalid next sound starting page");
                }
            }
            AIKO_ASSERT(lastPageIndex > firstPageIndex, "Invalid digitized sound page range");
            AIKO_ASSERT(lastPageIndex <= soundInfoChunkIndex, "Digitized sound range exceeds sound data pages");

            size_t totalPageBytes = 0;
            for (size_t pageIndex = firstPageIndex; pageIndex < lastPageIndex; ++pageIndex)
            {
                totalPageBytes += chunksLength[pageIndex];
            }
            if (totalPageBytes == 0)
            {
                continue;
            }

            uint16_t highWord = getHighWord(totalPageBytes);
            const uint16_t lowWord = getLowWord(totalPageBytes);
            if (highWord > 0 && lowWord < soundLengthLow)
            {
                --highWord;
            }
            const size_t soundLength = combineWords(highWord, soundLengthLow);
            AIKO_ASSERT(soundLength <= totalPageBytes, "Digitized sound length exceeds available page data");

            std::vector<uint8_t> samples;
            samples.reserve(soundLength);

            for (size_t pageIndex = firstPageIndex; pageIndex < lastPageIndex; ++pageIndex)
            {
                const uint32_t pageOffset = chunksOffset[pageIndex];
                const uint16_t pageLength = chunksLength[pageIndex];
                AIKO_ASSERT(pageOffset <= data.size(), "Digitized sound page offset out of bounds");
                AIKO_ASSERT(pageLength <= data.size() - pageOffset, "Digitized sound page exceeds file bounds");
                const size_t remainBytes = soundLength - samples.size();
                const size_t copyLength = std::min(size_t(pageLength), remainBytes);
                for (size_t sample = 0; sample < copyLength; ++sample)
                {
                    samples.push_back(data[pageOffset + sample]);
                }
                if (samples.size() == soundLength)
                {
                    break;
                }
            }
            AIKO_ASSERT(samples.size() == soundLength, "Digitized sound sample count mismatch");

            WolfDigitizedSound sound = {
                .samples = std::move(samples),
                .sampleRate = 7042,
            };
            sounds[soundIndex] = std::move(sound);

        }

        const WolfVSwapData result = {
            .walls = std::move(walls),
            .sprites = std::move(sprites),
            .sounds = std::move(sounds),
        };

        return result;

    }
}
