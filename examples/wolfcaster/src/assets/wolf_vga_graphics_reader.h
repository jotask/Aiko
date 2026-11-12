#pragma once

#include "wolf_vga_graphics_types.h"

#include <filesystem>

namespace wc
{
    class WolfVgaGraphicsReader final
    {
    public:
        static WolfVgaGraphicsData load(const std::filesystem::path& path);
        static std::vector<uint8_t> decodeChunk(const WolfVgaGraphicsData& graphics, size_t chunkIndex);
        static std::vector<WolfVgaPictureDimension> decodePictureDimensions(const WolfVgaGraphicsData& graphics, size_t pictureCount);
        static WolfVgaPicture decodePicture(const WolfVgaGraphicsData& graphics, size_t chunkIndex, WolfVgaPictureDimension dimension);
    private:
        static uint32_t findChunkEnd(const WolfVgaGraphicsData& graphics, size_t chunkIndex);
    };
}
