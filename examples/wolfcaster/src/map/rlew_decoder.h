#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace wc
{

    class RlewDecoder
    {
    public:
        static std::vector<uint16_t> decode(std::span<const uint16_t> data, uint16_t rlewFlag);
    };

}