#pragma once

#include <cstdint>
#include <vector>
#include <array>

namespace wc
{

    class WolfMap
    {
    public:

        enum class WolfMapPlane : uint8_t
        {
            Geometry = 0,
            Objects = 1,
            Info = 2,
        };

        WolfMap(uint16_t width, uint16_t height, std::array<std::vector<uint16_t>, 3>);

        uint16_t getWidth() const { return m_width; }
        uint16_t getHeight() const { return m_height; }
        uint16_t getTile(WolfMapPlane plane, uint16_t x, uint16_t y) const;
        uint16_t getTile(uint16_t x, uint16_t y) const;

    private:
        uint16_t m_width;
        uint16_t m_height;
        std::array<std::vector<uint16_t>, 3> m_planes;

    };

}