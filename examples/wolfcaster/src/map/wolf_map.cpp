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
        return m_planes[size_t(plane)][getTileIndex(x, y)];
    }

    WolfPlayerSpawn WolfMap::findPlayerSpawn() const
    {
        const auto& objectPlane = m_planes[size_t(WolfMapPlane::Objects)];
        for (size_t i = 0 ; i < objectPlane.size(); ++i)
        {
            const auto tileID = objectPlane[i];
            if (tileID >= 19 && tileID <= 22)
            {
                const WolfDirection direction = static_cast<WolfDirection>(tileID);
                return WolfPlayerSpawn{
                    .tileX = static_cast<uint16_t>(i % m_width),
                    .tileY = static_cast<uint16_t>(i / m_width),
                    .direction = direction,
                };
            }
        }
        AIKO_ASSERT(false, "No Player Spawn found!");
        return {};
    }

    bool WolfMap::isBlockingTile(int32_t x, int32_t y) const
    {
        if (x <  0 || x >= m_width || y < 0 ||  y >= m_height) // outside World?
        {
            return true;
        }
        const uint16_t tileId = getTile(WolfMapPlane::Geometry, x, y);
        if (tileId >= 1 && tileId <= 63) // Wall
        {
            return true;
        }
        if (tileId >= 90 && tileId <= 101) // FIXME door are considered wall for now
        {
            return false;
        }
        return false;
    }

    size_t WolfMap::getTileIndex(uint16_t x, uint16_t y) const
    {
        return y * size_t(m_width) + size_t(x);
    }
}
