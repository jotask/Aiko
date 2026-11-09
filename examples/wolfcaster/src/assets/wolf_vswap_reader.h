#pragma once

#include "wolf_vswap_types.h"

#include <filesystem>

namespace wc
{
    class WolfVSwapReader
    {
    public:
        static WolfVSwapData load(const std::filesystem::path& path);
    };
}
