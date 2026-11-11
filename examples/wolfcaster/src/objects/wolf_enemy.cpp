#include "wolf_enemy.h"

#include "map/wolf_map.h"

#include <cmath>
#include <algorithm>

namespace wc
{
    WolfEnemy::WolfEnemy(aiko::vec2 position, WolfEnemyType type, WolfDirection direction, WolfEnemySpawnMode mode)
        : m_position(position)
        , m_type(type)
        , m_direction(direction)
        , m_spawnMode(mode)
    {
        if (mode == WolfEnemySpawnMode::Patrolling)
        {
            transitionTo(WolfEnemyState::Patrolling);
        }
    }

    void WolfEnemy::transitionTo(WolfEnemyState state)
    {
        if (state == m_state)
        {
            return;
        }
        m_state = state;
        m_animationFrame = 0;
        m_animationTime = 0.0f;
        m_patrolTargetPosition = std::nullopt;
        switch (state)
        {
            case WolfEnemyState::Standing:
                m_animation = WolfEnemyAnimation::Idle;
                break;
            case WolfEnemyState::Patrolling:
                m_animation = WolfEnemyAnimation::Walk;
                break;
            case WolfEnemyState::Chasing:
                m_animation = WolfEnemyAnimation::Walk;
                break;
            case WolfEnemyState::Attacking:
                m_animation = WolfEnemyAnimation::Idle;
                break;
        }
    }

    void WolfEnemy::update(float deltaTime, WolfMap& map)
    {
        switch (m_state)
        {
            case WolfEnemyState::Standing: break;
            case WolfEnemyState::Patrolling: updatePatrolling(deltaTime, map); break;
            case WolfEnemyState::Attacking: break;
            case WolfEnemyState::Chasing: break;
        }

        constexpr float AnimationTick = 0.15f;
        if (m_animation == WolfEnemyAnimation::Idle)
        {
            m_animationFrame = 0;
            return;
        }
        m_animationTime += deltaTime;
        while (m_animationTime >= AnimationTick)
        {
            ++m_animationFrame;
            m_animationFrame %= 4;
            m_animationTime -= AnimationTick;
        }
    }

    void WolfEnemy::updatePatrolling(float deltaTime, WolfMap& map)
    {
        constexpr float PatrolSpeed = 0.55f;
        constexpr float EnemyRadius = 0.2f;

        float remainingMovement = PatrolSpeed * deltaTime;

        while (remainingMovement > 0)
        {
            aiko::vec2 moveDirection = {0};
            switch (m_direction)
            {
                case WolfDirection::North:      moveDirection = {0, -1}; break;
                case WolfDirection::NorthEast:  moveDirection = {1, -1}; break;
                case WolfDirection::East:       moveDirection = {1, 0}; break;
                case WolfDirection::SouthEast:  moveDirection = {1, 1}; break;
                case WolfDirection::South:      moveDirection = {0, 1}; break;
                case WolfDirection::SouthWest:  moveDirection = {-1, 1}; break;
                case WolfDirection::West:       moveDirection = {-1, 0}; break;
                case WolfDirection::NorthWest:  moveDirection = {-1, -1}; break;
            }

            if (m_patrolTargetPosition == std::nullopt)
            {
                const aiko::ivec2 currentTile = aiko::ivec2(std::floor(m_position.x), std::floor(m_position.y));
                const aiko::ivec2 newTile = currentTile + aiko::ivec2(moveDirection);

                if (newTile.x < 0 || newTile.y < 0 || newTile.x >= map.getWidth() || newTile.y >= map.getHeight())
                {
                    return;
                }

                const aiko::vec2 targetPosition = aiko::vec2(newTile) + 0.5f;

                if (const WolfDoor* door = map.getDoor(newTile.x, newTile.y))
                {
                    map.requestDoorOpen(newTile.x, newTile.y);
                    if (door->state != WolfDoorState::Open)
                    {
                        return;
                    }
                }

                if (map.collidesAt(targetPosition, EnemyRadius))
                {
                    return;
                }

                m_patrolTargetPosition = targetPosition;
            }

            const aiko::vec2 targetPosition = *m_patrolTargetPosition;
            const aiko::vec2 delta = targetPosition - m_position;
            const float distanceToTarget = std::max(std::abs(delta.x), std::abs(delta.y));
            const float movementStep = std::min(remainingMovement, distanceToTarget);

            const aiko::vec2 candidatePosition = m_position + (moveDirection * movementStep);
            if (map.collidesAt(candidatePosition, EnemyRadius))
            {
                return;
            }

            m_position = candidatePosition;
            remainingMovement -= movementStep;

            if (distanceToTarget <= movementStep + 0.0001f )
            {
                m_position = targetPosition;
                m_patrolTargetPosition = std::nullopt;

                const aiko::ivec2 tile = aiko::ivec2(std::floor(m_position.x), std::floor(m_position.y));
                const TileId marker = map.getObjectTile(tile.x, tile.y);
                if (marker >= 90 && marker <= 97)
                {
                    switch (marker)
                    {
                        case 90: m_direction = WolfDirection::East; break;
                        case 91: m_direction = WolfDirection::NorthEast; break;
                        case 92: m_direction = WolfDirection::North; break;
                        case 93: m_direction = WolfDirection::NorthWest; break;
                        case 94: m_direction = WolfDirection::West; break;
                        case 95: m_direction = WolfDirection::SouthWest; break;
                        case 96: m_direction = WolfDirection::South; break;
                        case 97: m_direction = WolfDirection::SouthEast; break;
                    }
                }

            }
            else
            {
                break;
            }

        }
    }

    WolfEnemy WolfEnemy::fromSpawn(const WolfEnemySpawn& spawn)
    {
        aiko::vec2 position = aiko::vec2(spawn.tile.x + 0.5f, spawn.tile.y + 0.5f );
        return WolfEnemy(position, spawn.type, spawn.direction, spawn.spawnMode);
    }
}
