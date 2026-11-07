#include "wolf_door_system.h"

namespace wc
{

    void WolfDoorSystem::initialize(const WolfMap& map)
    {
        m_doors.clear();

        for (uint16_t y = 0 ; y < map.getHeight(); ++y)
        {
            for (uint16_t x = 0 ; x < map.getWidth(); ++x)
            {
                const uint16_t tileId = map.getTile( WolfMapPlane::Geometry, x, y);
                if (tileId < 90 || tileId > 101 )
                {
                    continue;
                }

                WolfDoor door;

                door.tileCoord = {x, y};

                switch (tileId)
                {
                    case 90:
                    {
                        door.orientation = WolfDoorOrientation::Vertical;
                        door.kind = WolfDoorKind::Normal;
                    }
                    break;
                    case 91:
                    {
                        door.orientation = WolfDoorOrientation::Horizontal;
                        door.kind = WolfDoorKind::Normal;
                    }
                    break;
                    case 92:
                    case 94:
                    case 96:
                    case 98:
                    {
                        door.orientation = WolfDoorOrientation::Vertical;
                        door.kind = WolfDoorKind::Locked;
                    }
                    break;
                    case 93:
                    case 95:
                    case 97:
                    case 99:
                    {
                        door.orientation = WolfDoorOrientation::Horizontal;
                        door.kind = WolfDoorKind::Locked;
                    }
                    break;
                    case 100:
                    {
                        door.orientation = WolfDoorOrientation::Vertical;
                        door.kind = WolfDoorKind::Elevator;
                    }
                    break;
                    case 101:
                    {
                        door.orientation = WolfDoorOrientation::Horizontal;
                        door.kind = WolfDoorKind::Elevator;
                    }
                    break;
                        default:
                        AIKO_ASSERT(false, "I don't thin this is possible");
                }

                if (door.kind == WolfDoorKind::Locked)
                {
                    door.requiredKeyIndex = ((tileId - 92) / 2) + 1;
                }
                m_doors.push_back(door);
            }
        }

        aiko::logger::Log::infoFmt("Created {} doors", m_doors.size());

    }

    std::optional<size_t> WolfDoorSystem::findDoorIndex(int32_t tileX, int32_t tileY) const
    {
        for (size_t i = 0 ; i < m_doors.size(); ++i)
        {
            const WolfDoor& door = m_doors[i];
            if (door.tileCoord.x == tileX && door.tileCoord.y == tileY)
            {
                return i;
            }
        }
        return std::nullopt;
    }

    const WolfDoor& WolfDoorSystem::getDoor(size_t doorIndex) const
    {
        AIKO_ASSERT(doorIndex < m_doors.size(), "Door index OOB");
        return m_doors[doorIndex];
    }

}
