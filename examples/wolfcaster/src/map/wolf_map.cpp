#include "wolf_map.h"

#include "aiko_macros.h"

#include <utility>

namespace wc
{

    WolfMap::WolfMap(uint16_t width, uint16_t height, std::array<std::vector<uint16_t>, 3> planes)
        : m_width(width)
        , m_height(height)
        , m_planes(std::move(planes))
    {
        AIKO_ASSERT(m_width > 0 && m_height > 0, "Map dimension to small");
        for (const auto& plane : m_planes)
        {
            AIKO_ASSERT(size_t(m_width) * size_t(m_height) == plane.size(), "Map data received don't match");
        }
    }

    uint16_t WolfMap::getTile(WolfMapPlane plane, uint16_t x, uint16_t y) const
    {
        AIKO_ASSERT(size_t(plane) < m_planes.size(), "Unknow plane requested.");
        AIKO_ASSERT( x < m_width, "Requesting TileMap outside our X plane");
        AIKO_ASSERT( y < m_height, "Requesting TileMap outside our Y plane");
        return m_planes[size_t(plane)][y * size_t(m_width) + size_t(x)];
    }

    uint16_t WolfMap::getTile(uint16_t x, uint16_t y) const
    {
        return getTile(WolfMapPlane::Geometry, x, y);
    }
}
