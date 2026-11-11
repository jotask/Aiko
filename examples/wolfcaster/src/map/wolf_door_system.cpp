#include "wolf_door_system.h"

#include "math/math.h"
#include "wolf_map.h"

#include <algorithm>

namespace wc
{

    void WolfDoorSystem::initialize(const WolfMap& map)
    {
        m_doors.clear();
        m_activePushWallIndex.reset();

        for (uint16_t y = 0; y < map.getHeight(); ++y)
        {
            for (uint16_t x = 0; x < map.getWidth(); ++x)
            {
                const TileId tileId = map.getGeometryTile(x, y);
                const TileId objectId = map.getObjectTile(x, y);

                WolfDoor door;
                door.tileCoord = {x, y};

                // Secret push-wall marker is stored in the object plane.
                if (objectId == 98 && tileId >= 1 && tileId <= 63)
                {
                    door.kind = WolfDoorKind::PushWall;
                    door.wallTileId = tileId;
                    m_doors.push_back(door);
                    continue;
                }

                if (tileId < 90 || tileId > 101)
                {
                    continue;
                }

                switch (tileId)
                {
                    case 90:
                        door.orientation = WolfDoorOrientation::Vertical;
                        door.kind = WolfDoorKind::Normal;
                        break;
                    case 91:
                        door.orientation = WolfDoorOrientation::Horizontal;
                        door.kind = WolfDoorKind::Normal;
                        break;
                    case 92:
                    case 94:
                    case 96:
                    case 98:
                        door.orientation = WolfDoorOrientation::Vertical;
                        door.kind = WolfDoorKind::Locked;
                        break;
                    case 93:
                    case 95:
                    case 97:
                    case 99:
                        door.orientation = WolfDoorOrientation::Horizontal;
                        door.kind = WolfDoorKind::Locked;
                        break;
                    case 100:
                        door.orientation = WolfDoorOrientation::Vertical;
                        door.kind = WolfDoorKind::Elevator;
                        break;
                    case 101:
                        door.orientation = WolfDoorOrientation::Horizontal;
                        door.kind = WolfDoorKind::Elevator;
                        break;
                    default:
                        AIKO_ASSERT(false, "Unknown door tile");
                }

                if (door.kind == WolfDoorKind::Locked)
                {
                    door.requiredKeyIndex = ((tileId - 92) / 2) + 1;
                }

                m_doors.push_back(door);
            }
        }
        aiko::logger::Log::infoFmt("Created {} doors and push-walls", m_doors.size());
    }

    void WolfDoorSystem::update(float deltaTime, WolfMap& map, const aiko::vec2& playerPosition)
    {
        constexpr float TimeToOpenCloseDoor = 2.5f;
        constexpr float TimeToAutoClose = 3.0f;
        constexpr float ActorRadius = 0.2f;

        const auto overlapsDoor = [&](const WolfDoor& door, const aiko::vec2& position) -> bool
        {
            const float x = static_cast<float>(door.tileCoord.x);
            const float y = static_cast<float>(door.tileCoord.y);

            const aiko::vec2 closestPoint = {
                aiko::math::clamp(position.x, x, x + 1.0f),
                aiko::math::clamp(position.y, y, y + 1.0f)
            };

            return aiko::math::distanceSquared(position, closestPoint) <= ActorRadius * ActorRadius;
        };

        const auto isDoorOccupied = [&](const WolfDoor& door) -> bool
        {
            if (overlapsDoor(door, playerPosition))
            {
                return true;
            }

            for (const WolfEnemy& enemy : map.getEnemies())
            {
                if (overlapsDoor(door, enemy.position()))
                {
                    return true;
                }
            }

            return false;
        };

        for (WolfDoor& door : m_doors)
        {
            if (door.kind == WolfDoorKind::PushWall)
            {
                continue;
            }
            switch (door.state)
            {
                case WolfDoorState::Closed:
                    break;
                case WolfDoorState::Opening:
                    {
                        door.openProgress += deltaTime / TimeToOpenCloseDoor;
                        door.openProgress = aiko::math::clamp(door.openProgress, 0.0f, 1.0f);
                        if (door.openProgress >= 1.0f)
                        {
                            door.state = WolfDoorState::Open;
                            door.timer = 0.0f;
                        }
                    }
                    break;
                case WolfDoorState::Open:
                    {
                        door.timer += deltaTime;
                        if (door.timer >= TimeToAutoClose && !isDoorOccupied(door))
                        {
                            door.state = WolfDoorState::Closing;
                            door.timer = 0.0f;
                        }
                    }
                    break;
                case WolfDoorState::Closing:
                {
                    if (isDoorOccupied(door))
                    {
                        door.state = WolfDoorState::Opening;
                        door.timer = 0.0f;
                        break;
                    }
                    door.openProgress -= deltaTime / TimeToOpenCloseDoor;
                    door.openProgress = aiko::math::clamp(door.openProgress, 0.0f, 1.0f);
                    if (door.openProgress <= 0.0f)
                    {
                        door.state = WolfDoorState::Closed;
                        door.timer = 0.0f;
                    }
                    break;
                }
            }
        }
        if (m_activePushWallIndex.has_value())
        {
            updatePushWall(m_doors[*m_activePushWallIndex], deltaTime, map, playerPosition);
        }
    }

    std::optional<size_t> WolfDoorSystem::findDoorIndex(int32_t tileX, int32_t tileY) const
    {
        for (size_t i = 0; i < m_doors.size(); ++i)
        {
            const WolfDoor& door = m_doors[i];
            if (door.kind == WolfDoorKind::PushWall)
            {
                continue;
            }
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
            if (door.kind == WolfDoorKind::PushWall)
            {
                if (door.state != WolfDoorState::Opening)
                {
                    continue;
                }

                const aiko::vec2 boxMin = {
                    float(door.tileCoord.x) + door.pushDirection.x * door.openProgress,
                    float(door.tileCoord.y) + door.pushDirection.y * door.openProgress
                };

                const aiko::vec2 boxMax = boxMin + aiko::vec2(1.0f);

                const aiko::vec2 closestPoint = {
                    aiko::math::clamp(position.x, boxMin.x, boxMax.x),
                    aiko::math::clamp(position.y, boxMin.y, boxMax.y)
                };

                if (aiko::math::distanceSquared(position, closestPoint) < radius * radius)
                {
                    return true;
                }

                continue;
            }

            if (door.state == WolfDoorState::Open)
            {
                continue;
            }

            const float tileX = float(door.tileCoord.x);
            const float tileY = float(door.tileCoord.y);

            aiko::vec2 closestPoint = {0.0f};

            switch (door.orientation)
            {
                case WolfDoorOrientation::Horizontal:
                    closestPoint = { aiko::math::clamp(position.x, tileX + door.openProgress, tileX + 1.0f), tileY + 0.5f };
                    break;
                case WolfDoorOrientation::Vertical:
                    closestPoint = { tileX + 0.5f, aiko::math::clamp(position.y, tileY + door.openProgress, tileY + 1.0f) };
                    break;
                default:
                    AIKO_ASSERT(false, "Unknown door orientation");
            }

            if (aiko::math::distanceSquared(position, closestPoint) < radius * radius)
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

    void WolfDoorSystem::openDoor(size_t doorIndex)
    {
        AIKO_ASSERT(doorIndex < m_doors.size(), "Door OOB");
        WolfDoor& door = m_doors[doorIndex];
        if (door.state == WolfDoorState::Closed || door.state == WolfDoorState::Closing)
        {
            door.state = WolfDoorState::Opening;
            door.timer = 0.0f;
        }
        else if (door.state == WolfDoorState::Open)
        {
            door.timer = 0.0f;
        }
    }

    bool WolfDoorSystem::activatePushWall(int32_t tileX, int32_t tileY, const aiko::ivec2& direction, WolfMap& map, const aiko::vec2& playerPosition)
    {
        if (m_activePushWallIndex.has_value() == true)
        {
            return false;
        }

        const auto wallIndex = findPushWallIndex(tileX, tileY);
        if (wallIndex.has_value() == false)
        {
            return false;
        }

        WolfDoor& wall = m_doors[*wallIndex];
        if (wall.state != WolfDoorState::Closed || map.isPushWall(tileX, tileY) == false)
        {
            return false;
        }

        const bool validDirection =
            ((direction.x == 1 || direction.x == -1) && direction.y == 0) ||
            ((direction.y == 1 || direction.y == -1) && direction.x == 0);

        if (validDirection == false)
        {
            return false;
        }

        const int32_t nextX = tileX + direction.x;
        const int32_t nextY = tileY + direction.y;

        if (canPushWallInto(map, nextX, nextY, playerPosition) == false)
        {
            return false;
        }

        wall.pushDirection = direction;
        wall.openProgress = 0.0f;
        wall.state = WolfDoorState::Opening;

        m_activePushWallIndex = *wallIndex;

        map.setGeometryTile(tileX, tileY, 0);
        map.setObjectTile(tileX, tileY, 0);

        aiko::logger::Log::infoFmt("Secret wall activated at ({}, {}), direction ({}, {})", tileX, tileY, direction.x, direction.y);

        return true;
    }

    const WolfDoor* WolfDoorSystem::getActivePushWall() const
    {
        if (m_activePushWallIndex.has_value() == false)
        {
            return nullptr;
        }
        return &m_doors[*m_activePushWallIndex];
    }

    std::optional<size_t> WolfDoorSystem::findPushWallIndex(int32_t tileX, int32_t tileY) const
    {
        for (size_t i = 0; i < m_doors.size(); ++i)
        {
            const WolfDoor& door = m_doors[i];
            if (door.kind == WolfDoorKind::PushWall && door.tileCoord.x == tileX && door.tileCoord.y == tileY)
            {
                return i;
            }
        }
        return std::nullopt;
    }

    bool WolfDoorSystem::canPushWallInto(const WolfMap& map, int32_t tileX, int32_t tileY, const aiko::vec2& playerPosition) const
    {
        if (tileX < 0 || tileY < 0 || tileX >= map.getWidth() || tileY >= map.getHeight())
        {
            return false;
        }

        if (map.isBlockingTile(tileX, tileY))
        {
            return false;
        }

        if (findDoorIndex(tileX, tileY).has_value())
        {
            return false;
        }

        const auto overlapsTile = [&](const aiko::vec2& position, float radius)
        {
            const aiko::vec2 closestPoint = {
                aiko::math::clamp(position.x, float(tileX), float(tileX + 1)),
                aiko::math::clamp(position.y, float(tileY), float(tileY + 1))
            };

            return aiko::math::distanceSquared(position, closestPoint) < radius * radius;
        };

        if (overlapsTile(playerPosition, 0.2f))
        {
            return false;
        }

        for (const WolfEnemy& enemy : map.getEnemies())
        {
            if (enemy.isAlive() && overlapsTile(enemy.position(), WolfEnemy::EnemyRadius))
            {
                return false;
            }
        }

        constexpr float ObjectRadius = 0.2f;

        for (const WolfWorldObject& object : map.getObjects())
        {
            if (object.blocking && overlapsTile(object.position, ObjectRadius))
            {
                return false;
            }
        }

        return true;
    }

    void WolfDoorSystem::updatePushWall(WolfDoor& wall, float deltaTime, WolfMap& map, const aiko::vec2& playerPosition)
    {
        if (wall.state != WolfDoorState::Opening)
        {
            return;
        }

        constexpr float PushWallSpeed = 1.0f;

        float remainingMovement = std::max(0.0f, deltaTime) * PushWallSpeed;

        while (remainingMovement > 0.0f)
        {
            const float segmentEnd = wall.openProgress < 1.0f ? 1.0f : 2.0f;
            const float movement = std::min(remainingMovement, segmentEnd - wall.openProgress);

            wall.openProgress += movement;
            remainingMovement -= movement;

            if (wall.openProgress < segmentEnd)
            {
                break;
            }

            wall.openProgress = segmentEnd;

            if (segmentEnd == 1.0f)
            {
                const int32_t secondX = int32_t(wall.tileCoord.x) + wall.pushDirection.x * 2;
                const int32_t secondY = int32_t(wall.tileCoord.y) + wall.pushDirection.y * 2;

                if (canPushWallInto(map, secondX, secondY, playerPosition))
                {
                    continue;
                }
            }

            const int32_t finalX = int32_t(wall.tileCoord.x) + wall.pushDirection.x * int32_t(segmentEnd);
            const int32_t finalY = int32_t(wall.tileCoord.y) + wall.pushDirection.y * int32_t(segmentEnd);

            map.setGeometryTile(finalX, finalY, wall.wallTileId);

            wall.state = WolfDoorState::Open;
            m_activePushWallIndex.reset();

            break;
        }
    }

}
