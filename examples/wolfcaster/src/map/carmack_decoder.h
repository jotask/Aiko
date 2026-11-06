#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace wc
{
    class CarmackDecoder
    {
    public:
        static std::vector<uint16_t> decode(std::span<const uint8_t> data);
    };
}