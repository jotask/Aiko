#pragma once

#include "wolf_vga_graphics_types.h"

#include <filesystem>

namespace wc
{
    class WolfVgaGraphicsReader final
    {
    public:
        static WolfVgaGraphicsData load(const std::filesystem::path& path);
    };
}
