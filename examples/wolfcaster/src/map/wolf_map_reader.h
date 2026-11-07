#pragma once

#include "wolf_map.h"

#include <filesystem>
#include <optional>
#include <span>

namespace wc
{
    class WolfMapReader
    {
    public:

        static std::optional<WolfMap> load(const std::filesystem::path& mapHead, const std::filesystem::path& gameMapPath);

    private:

        struct WolfLevelHeader
        {
            std::array<std::pair<uint32_t, uint16_t>, 3> planesOffset; // offset and length
            uint16_t width;
            uint16_t height;
            std::string name;
        };

        static WolfLevelHeader readLevelHeader(std::span<const uint8_t> data, size_t offset);

        static std::vector<uint16_t> decodePlane(std::span<const uint8_t> gameMapBytes, uint32_t offset, uint16_t compressedLength, uint16_t rlewTag, size_t expectedTileCount);

    };
}