#pragma once

#include "wolf_map_types.h"

#include <array>
#include <cstdint>
#include <vector>

namespace wc
{

    class WolfMap
    {
    public:

        WolfMap(uint16_t width, uint16_t height, std::array<std::vector<uint16_t>, 3>);

        uint16_t getWidth() const { return m_width; }
        uint16_t getHeight() const { return m_height; }
        uint16_t getTile(WolfMapPlane plane, uint16_t x, uint16_t y) const;
        uint16_t getTile(uint16_t x, uint16_t y) const;

        WolfPlayerSpawn findPlayerSpawn() const;

    private:

        uint16_t m_width;
        uint16_t m_height;
        std::array<std::vector<uint16_t>, 3> m_planes;

    };

}