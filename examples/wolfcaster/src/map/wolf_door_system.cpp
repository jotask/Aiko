#include "wolf_door_system.h"

#include "math/math.h"
#include "wolf_map.h"

namespace wc
{

    void WolfDoorSystem::initialize(const WolfMap& map)
    {
        m_doors.clear();

        for (uint16_t y = 0 ; y < map.getHeight(); ++y)
        {
            for (uint16_t x = 0 ; x < map.getWidth(); ++x)
            {
                const uint16_t tileId = map.getTile(WolfMapPlane::Geometry, x, y);
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

    void WolfDoorSystem::update(float deltaTime)
    {
        constexpr float TimeToOpenCloseDoor = 2.5f;
        for (WolfDoor& door : m_doors)
        {
            if (door.state == WolfDoorState::Opening)
            {
                door.timer += deltaTime;
                door.timer = aiko::math::clamp(door.timer, 0.0f, TimeToOpenCloseDoor);
                door.openProgress += deltaTime / TimeToOpenCloseDoor;
                door.openProgress = aiko::math::clamp(door.openProgress, 0.0f, 1.0f);
                if (door.openProgress >= 1.0f)
                {
                    door.state = WolfDoorState::Open;
                    continue;
                }
            }
            else if (door.state == WolfDoorState::Closing)
            {
                door.timer += deltaTime;
                door.timer = aiko::math::clamp(door.timer, 0.0f, TimeToOpenCloseDoor);
                door.openProgress -= deltaTime / TimeToOpenCloseDoor;
                door.openProgress = aiko::math::clamp(door.openProgress, 0.0f, 1.0f);
                if (door.openProgress <= 0.0f)
                {
                    door.state = WolfDoorState::Closed;
                    continue;
                }
            }
        }
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

    bool WolfDoorSystem::collidesAt(const aiko::vec2& position, float radius) const
    {
        for (const WolfDoor& door : m_doors)
        {

            if (door.state == WolfDoorState::Open)
            {
                continue;
            }

            const float tileX = static_cast<float>(door.tileCoord.x);
            const float tileY = static_cast<float>(door.tileCoord.y);

            aiko::vec2 closestPoint = {0.0f};

            switch (door.orientation)
            {
                case WolfDoorOrientation::Horizontal:
                {
                    closestPoint = {
                        aiko::math::clamp(position.x, tileX + door.openProgress, tileX + 1.0f ),
                        tileY + 0.5f,
                    };
                }
                break;
                case WolfDoorOrientation::Vertical:
                {
                    closestPoint = {
                        tileX + 0.5f,
                        aiko::math::clamp(position.y, tileY + door.openProgress, tileY + 1.0f ),
                    };
                }
                break;
                default:
                    AIKO_ASSERT(false, "Unknown door orientation");
            }

            const float distanceSquared = aiko::math::distanceSquared(position, closestPoint);
            if (distanceSquared < radius * radius )
            {
                return true;
            }
        }
        return false;
    }

    void WolfDoorSystem::toggleDoor(size_t doorIndex)
    {
        AIKO_ASSERT(doorIndex < m_doors.size(), "Door OOB");
        WolfDoor& door = m_doors[doorIndex];
        if (door.state == WolfDoorState::Closed || door.state == WolfDoorState::Closing)
        {
            door.state = WolfDoorState::Opening;
            aiko::logger::Log::info("Opening");
        }
        else  if (door.state == WolfDoorState::Open || door.state == WolfDoorState::Opening)
        {
            door.state = WolfDoorState::Closing;
            aiko::logger::Log::info("Closing");
        }
    }

}
