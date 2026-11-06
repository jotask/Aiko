#include "wolf_map_reader.h"

#include "aiko_macros.h"
#include "byte_utils.h"
#include "carmack_decoder.h"
#include "core/file.h"
#include "logger/logger.h"
#include "rlew_decoder.h"
#include "wolf_map.h"

namespace wc
{

    std::optional<WolfMap> WolfMapReader::load(const std::filesystem::path& mapHead, const std::filesystem::path& gameMapPath)
    {
        AIKO_ASSERT(std::filesystem::exists(mapHead), "MAPHEAD don't exist");
        AIKO_ASSERT(std::filesystem::exists(gameMapPath), "GAMEMAP don't exist");

        const std::vector<std::uint8_t> mapHeadBytes = aiko::files::readFileBytes(mapHead);
        const auto rlewTag = readU16LE(mapHeadBytes, 0);
        const auto levelOneOffset = readU32LE(mapHeadBytes, 2);

        aiko::logger::Log::infoFmt("RLEW Tag: 0x{:04X}", rlewTag);
        aiko::logger::Log::infoFmt("Level 0 Offset: {} (0x{:08X})", levelOneOffset, levelOneOffset);

        const std::vector<std::uint8_t> gameMapBytes = aiko::files::readFileBytes(gameMapPath);

        const WolfLevelHeader levelInfo = readLevelHeader(gameMapBytes, levelOneOffset);
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

        return WolfMap(levelInfo.width, levelInfo.height, std::move(planes));

    }

    WolfMapReader::WolfLevelHeader WolfMapReader::readLevelHeader(std::span<const uint8_t> data, size_t offset)
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

    std::vector<uint16_t> WolfMapReader::decodePlane(std::span<const uint8_t> gameMapBytes, uint32_t offset, uint16_t compressedLength, uint16_t rlewTag, size_t expectedTileCount)
    {
        AIKO_ASSERT(offset <= gameMapBytes.size() && compressedLength <= gameMapBytes.size() - offset, "Plane data exceeds GAMEMAPS file boundaries");
        std::span<const uint8_t> plane = {gameMapBytes.begin() + offset, compressedLength};
        const std::vector<uint16_t> decodedCarmackPlane = CarmackDecoder::decode(plane);
        std::vector<uint16_t> decodedRlewPlane = RlewDecoder::decode(decodedCarmackPlane, rlewTag);
        AIKO_ASSERT(decodedRlewPlane.size() == expectedTileCount, "Decoded plane size does not match map dimensions");
        return decodedRlewPlane;
    }
}
