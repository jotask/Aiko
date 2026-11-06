#pragma once
#include <filesystem>
#include <optional>
#include <span>

namespace wc
{
    class WolfMapReader
    {
    public:

        bool load(const std::filesystem::path& mapHead, const std::filesystem::path& gameMapPath);

    private:

        struct WolfLevelHeader
        {
            std::array<std::pair<uint32_t, uint16_t>, 3> planesOffset; // offset and length
            uint16_t width;
            uint16_t height;
            std::string name;
        };

        uint16_t readU16LE(std::span<const uint8_t> data, size_t offset);
        uint32_t readU32LE(std::span<const uint8_t> data, size_t offset);

        WolfLevelHeader readLevelHeader(std::span<const uint8_t> data, size_t offset);

    };
}