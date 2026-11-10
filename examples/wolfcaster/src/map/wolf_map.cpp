#include "wolf_map.h"

#include "aiko_macros.h"
#include "math/math.h"

#include <utility>

namespace wc
{

    WolfMap::WolfMap(WolfMapData data)
        : m_data(std::move(data))
    {
        AIKO_ASSERT(getWidth() > 0 && getHeight() > 0, "Invalid map dimensions");

        const size_t expectedTileCount = size_t(getWidth()) * size_t(getHeight());
        AIKO_ASSERT(m_data.geometry.size() == expectedTileCount, "Invalid geometry size");
        AIKO_ASSERT(m_data.info.size() == expectedTileCount, "Invalid info size");

        m_doorSystem.initialize(*this);

    }

    void WolfMap::update(float deltaTime)
    {
        m_doorSystem.update(deltaTime);
    }

    TileId WolfMap::getGeometryTile(uint16_t x, uint16_t y) const
    {
        AIKO_ASSERT(x < getWidth(), "Geometry tile X out of bounds");
        AIKO_ASSERT(y < getHeight(), "Geometry tile Y out of bounds");
        return m_data.geometry[getTileIndex(x, y)];
    }

    bool WolfMap::canInteract(int32_t tileX, int32_t tileY) const
    {
        if (tileX <  0 || tileX >= getWidth() || tileY < 0 || tileY >= getHeight()) // outside World?
        {
            return false;
        }
        if (m_doorSystem.findDoorIndex(tileX, tileY) != std::nullopt)
        {
            return true;
        }
        return false;
    }

    bool WolfMap::interact(int32_t tileX, int32_t tileY)
    {
        if (canInteract(tileX, tileY) == false)
        {
            return false;
        }
        const auto doorIndex = m_doorSystem.findDoorIndex(tileX, tileY);
        if (doorIndex != std::nullopt)
        {
            m_doorSystem.toggleDoor(doorIndex.value());
            return true;
        }
        return false;
    }

    bool WolfMap::isBlockingTile(int32_t x, int32_t y) const
    {
        if (x <  0 || x >= getWidth() || y < 0 ||  y >= getHeight()) // outside World?
        {
            return true;
        }
        const TileId tileId = getGeometryTile(x, y);
        if (tileId >= 1 && tileId <= 63) // Wall
        {
            return true;
        }
        if (tileId >= 90 && tileId <= 101)
        {
            return false;
        }
        return false;
    }

    bool WolfMap::collidesAt(const aiko::vec2& position, float radius) const
    {
        const int32_t minTileX = std::floor(position.x - radius);
        const int32_t maxTileX = std::floor(position.x + radius);
        const int32_t minTileY = std::floor(position.y - radius);
        const int32_t maxTileY = std::floor(position.y + radius);

        for (int32_t y = minTileY ; y <= maxTileY ; ++y)
        {
            for (int32_t x = minTileX ; x <= maxTileX ; ++x)
            {
                if (isBlockingTile(x, y) == false)
                {
                    continue;
                }
                const aiko::vec2 closestPoint =
                {
                    aiko::math::clamp(position.x, static_cast<float>(x), static_cast<float>(x + 1) ),
                    aiko::math::clamp(position.y, static_cast<float>(y), static_cast<float>(y + 1) ),
                };
                const float distanceSquared = aiko::math::distanceSquared(position, closestPoint);
                if (distanceSquared <  radius * radius)
                {
                    return true;
                }
            }
        }
        return m_doorSystem.collidesAt(position, radius);
    }

    const WolfDoor* WolfMap::getDoor(int32_t tileX, int32_t tileY) const
    {
        const auto doorIdx = m_doorSystem.findDoorIndex(tileX, tileY);
        if (doorIdx == std::nullopt)
        {
            return nullptr;
        }
        const WolfDoor& door = m_doorSystem.getDoor(*doorIdx);
        return &door;
    }

    size_t WolfMap::getTileIndex(uint16_t x, uint16_t y) const
    {
        return size_t(y) * getWidth() + size_t(x);
    }
}
