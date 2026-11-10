#include "wolf_map_loader.h"

#include "aiko_macros.h"
#include "byte_utils.h"
#include "carmack_decoder.h"
#include "core/file.h"
#include "logger/logger.h"
#include "rlew_decoder.h"
#include "wolf_map.h"

namespace wc
{

    namespace
    {
        constexpr const char* MapHead = "MAPHEAD.WL1";
        constexpr const char* GameMaps = "GAMEMAPS.WL1";
    }

    WolfMap WolfMapLoader::loadLevel(const std::filesystem::path& dataDirectory, uint16_t levelIndex)
    {

        const std::filesystem::path mapHeadPath = dataDirectory / MapHead;
        const std::filesystem::path gameMapsPath = dataDirectory / GameMaps;

        AIKO_ASSERT(std::filesystem::exists(mapHeadPath), "MAPHEAD don't exist");
        AIKO_ASSERT(std::filesystem::exists(gameMapsPath), "GAMEMAPS don't exist");

        const std::vector<std::uint8_t> mapHeadBytes = aiko::files::readFileBytes(mapHeadPath);
        AIKO_ASSERT(mapHeadBytes.size() >= 2, "Invalid MAPHEAD");

        const uint16_t rlewTag = readU16LE(mapHeadBytes, 0);
        const size_t offsetPosition = 2 + size_t(levelIndex) * 4;
        AIKO_ASSERT(offsetPosition <= mapHeadBytes.size() && mapHeadBytes.size() - offsetPosition >= 4, "Invalid level index");
        const uint32_t levelOffset = readU32LE(mapHeadBytes, offsetPosition);
        AIKO_ASSERT(levelOffset != 0 && levelOffset != UINT32_MAX, "Level is unavailable");

        aiko::logger::Log::infoFmt("RLEW Tag: 0x{:04X}", rlewTag);
        aiko::logger::Log::infoFmt("Level {} Offset: {} (0x{:08X})", levelIndex, levelOffset, levelOffset);

        const std::vector<std::uint8_t> gameMapBytes = aiko::files::readFileBytes(gameMapsPath);
        AIKO_ASSERT(levelOffset <= gameMapBytes.size() && gameMapBytes.size() - levelOffset >= 38, "Invalid level header offset");

        const WolfLevelHeader levelInfo = readLevelHeader(gameMapBytes, levelOffset);
        const size_t expectedTileCount = size_t(levelInfo.width) * size_t(levelInfo.height);

        std::vector<uint16_t> plane0 = decodePlane(gameMapBytes, levelInfo.planesOffset[0].first, levelInfo.planesOffset[0].second, rlewTag, expectedTileCount);
        std::vector<uint16_t> plane1 = decodePlane(gameMapBytes, levelInfo.planesOffset[1].first, levelInfo.planesOffset[1].second, rlewTag, expectedTileCount);
        std::vector<uint16_t> plane2;
        if (levelInfo.planesOffset[2].second > 0)
        {
            plane2 = decodePlane(gameMapBytes, levelInfo.planesOffset[2].first, levelInfo.planesOffset[2].second, rlewTag, expectedTileCount);
        }
        else
        {
            plane2.resize(expectedTileCount, 0);
        }

        std::array<std::vector<uint16_t>, 3> planes = {
            std::move(plane0),
            std::move(plane1),
            std::move(plane2),
        };

        return WolfMap(levelIndex, levelInfo.name, levelInfo.width, levelInfo.height, std::move(planes));

    }

    std::vector<WolfLevelInfo> WolfMapLoader::getAvailableLevels(const std::filesystem::path& dataDirectory)
    {
        const std::filesystem::path mapHeadPath = dataDirectory / MapHead;
        const std::filesystem::path gameMapsPath = dataDirectory / GameMaps;

        AIKO_ASSERT(std::filesystem::exists(mapHeadPath), "MAPHEAD doesn't exist");
        AIKO_ASSERT(std::filesystem::exists(gameMapsPath), "GAMEMAPS doesn't exist");

        const std::vector<uint8_t> mapHeadBytes = aiko::files::readFileBytes(mapHeadPath);
        const std::vector<uint8_t> gameMapBytes = aiko::files::readFileBytes(gameMapsPath);

        AIKO_ASSERT(mapHeadBytes.size() >= 2, "Invalid MAPHEAD");

        const size_t levelCount = (mapHeadBytes.size() - 2) / 4;

        std::vector<WolfLevelInfo> availableLevels;

        for (size_t i = 0; i < levelCount && i <= UINT16_MAX; ++i)
        {
            const uint32_t levelOffset = readU32LE(mapHeadBytes, 2 + i * 4);

            if (levelOffset == 0 || levelOffset == UINT32_MAX)
            {
                continue;
            }

            if (levelOffset > gameMapBytes.size() ||
                gameMapBytes.size() - levelOffset < 38)
            {
                continue;
            }

            const uint16_t width = readU16LE(gameMapBytes, levelOffset + 18);
            const uint16_t height = readU16LE(gameMapBytes, levelOffset + 20);

            if (width == 0 || height == 0)
            {
                continue;
            }

            bool valid = true;

            for (size_t plane = 0; plane < 3; ++plane)
            {
                const uint32_t offset = readU32LE(gameMapBytes, levelOffset + plane * 4);
                const uint16_t length = readU16LE(gameMapBytes, levelOffset + 12 + plane * 2);
                if (plane == 2 && length == 0)
                {
                    continue;
                }
                if (length == 0 || offset > gameMapBytes.size() || gameMapBytes.size() - offset < length)
                {
                    valid = false;
                    break;
                }
            }
            if (valid)
            {

                const WolfLevelHeader header = readLevelHeader(gameMapBytes, levelOffset);

                availableLevels.push_back({
                    .index = static_cast<uint16_t>(i),
                    .name = header.name,
                    .width = header.width,
                    .height = header.height
                });
            }
        }

        return availableLevels;

    }

    WolfMapLoader::WolfLevelHeader WolfMapLoader::readLevelHeader(std::span<const uint8_t> data, size_t offset)
    {
        WolfLevelHeader info;

        info.planesOffset[0] = {readU32LE(data, offset + 0), readU16LE(data, offset + 12)};
        info.planesOffset[1] = {readU32LE(data, offset + 4), readU16LE(data, offset + 14)};
        info.planesOffset[2] = {readU32LE(data, offset + 8), readU16LE(data, offset + 16)};

        info.width = readU16LE(data, offset + 18);
        info.height = readU16LE(data, offset + 20);

        std::vector<uint8_t> name;
        for (size_t i = 0 ; i < 16 ; ++i)
        {
            uint8_t byte = data[offset + 22 + i];
            if (byte == 0x00)
            {
                break;
            }
            name.push_back(byte);
        }

        info.name = std::string(name.begin(), name.end());

        aiko::logger::Log::infoFmt("Level Header Info");
        aiko::logger::Log::infoFmt("     Name: {}", info.name);
        aiko::logger::Log::infoFmt("     Width: {} Height: {}", info.width, info.height);
        for (size_t i = 0 ; i < info.planesOffset.size(); ++i)
        {
            aiko::logger::Log::infoFmt("     Plane {}: Offset {} CompressedLength: {} ", i, info.planesOffset[i].first, info.planesOffset[i].second);
        }

        return info;
    }

    std::vector<uint16_t> WolfMapLoader::decodePlane(std::span<const uint8_t> gameMapBytes, uint32_t offset, uint16_t compressedLength, uint16_t rlewTag, size_t expectedTileCount)
    {
        AIKO_ASSERT(offset <= gameMapBytes.size() && compressedLength <= gameMapBytes.size() - offset, "Plane data exceeds GAMEMAPS file boundaries");
        std::span<const uint8_t> plane = {gameMapBytes.begin() + offset, compressedLength};
        const std::vector<uint16_t> decodedCarmackPlane = CarmackDecoder::decode(plane);
        std::vector<uint16_t> decodedRlewPlane = RlewDecoder::decode(decodedCarmackPlane, rlewTag);
        AIKO_ASSERT(decodedRlewPlane.size() == expectedTileCount, "Decoded plane size does not match map dimensions");
        return decodedRlewPlane;
    }
}
