#pragma once

#include <cstdint>
#include <vector>

namespace wc
{

    class WolfMap
    {
    public:

        WolfMap(uint16_t width, uint16_t height, std::vector<uint16_t> tiles);

        uint16_t getWidth() const { return m_width; }
        uint16_t getHeight() const { return m_height; }
        uint16_t getTile(uint16_t x, uint16_t y) const;

    private:
        uint16_t m_width;
        uint16_t m_height;
        std::vector<uint16_t> m_tiles;

    };

}