#pragma once

#include "wolf_map.h"

#include <filesystem>
#include <optional>
#include <span>

namespace wc
{
    class WolfMapLoader
    {
    public:

        static WolfMap loadLevel(const std::filesystem::path& dataDirectory, uint16_t levelIndex);

        static std::vector<WolfLevelInfo> getAvailableLevels(const std::filesystem::path& dataDirectory);

    private:

        struct WolfLevelHeader
        {
            uint16_t width;
            uint16_t height;
            std::string name;
            std::array<std::pair<uint32_t, uint16_t>, 3> planesOffset; // offset and length
        };

        static WolfLevelHeader readLevelHeader(std::span<const uint8_t> data, size_t offset);

        static std::vector<uint16_t> decodePlane(std::span<const uint8_t> gameMapBytes, uint32_t offset, uint16_t compressedLength, uint16_t rlewTag, size_t expectedTileCount);

        static std::optional<WolfEnemySpawn> parseEnemySpawn(TileId objectId, Tile tile);

        static bool isBlockingStaticObject(TileId objectId);

    };
}