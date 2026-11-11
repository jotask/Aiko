#include "wolf_enemy.h"

#include "core/converters.h"
#include "map/wolf_map.h"
#include "math/math.h"
#include "raycasting/wolf_raycaster.h"

#include <algorithm>
#include <cmath>

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

    void WolfEnemy::update(float deltaTime, WolfMap& map, const aiko::vec2& playerPosition)
    {
        switch (m_state)
        {
            case WolfEnemyState::Standing: updateStanding(deltaTime, map, playerPosition); break;
            case WolfEnemyState::Patrolling: updatePatrolling(deltaTime, map, playerPosition); break;
            case WolfEnemyState::Chasing: updateChasing(deltaTime, map, playerPosition); break;
            case WolfEnemyState::Attacking: updateAttacking(deltaTime, map, playerPosition); break;
        }

        constexpr float AnimationTick = 0.15f;

        if (m_animation == WolfEnemyAnimation::Idle)
        {
            m_animationFrame = 0;
            m_animationTime = 0.0f;
            return;
        }

        m_animationTime += deltaTime;

        while (m_animationTime >= AnimationTick)
        {
            m_animationTime -= AnimationTick;

            if (m_animation == WolfEnemyAnimation::Walk)
            {
                m_animationFrame = (m_animationFrame + 1) % 4;
            }
            else if (m_animation == WolfEnemyAnimation::Attack)
            {
                if (m_animationFrame + 1 >= getAttackFrameCount())
                {
                    m_animation = WolfEnemyAnimation::Idle;
                    m_animationFrame = 0;
                    m_animationTime = 0.0f;
                    break;
                }

                ++m_animationFrame;
                if (m_animationFrame == 1)
                {
                    m_shotPending = true;
                }
            }
        }
    }

    void WolfEnemy::updateStanding(float deltaTime, WolfMap& map, const aiko::vec2& playerPosition)
    {
        tryDetectPlayer(map, playerPosition);
    }

    void WolfEnemy::updatePatrolling(float deltaTime, WolfMap& map, const aiko::vec2& playerPosition)
    {
        constexpr float PatrolSpeed = 0.55f;

        if (tryDetectPlayer(map, playerPosition))
        {
            return;
        }

        float remainingMovement = PatrolSpeed * deltaTime;

        while (remainingMovement > 0)
        {
            const aiko::vec2 moveDirection = convertDirectionToVector(m_direction);

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

    void WolfEnemy::updateAttacking(float deltaTime, WolfMap& map, const aiko::vec2& playerPosition)
    {
        constexpr float Epsilon = 0.001f;

        if (canAttackPlayer(map, playerPosition) == false)
        {
            transitionTo(WolfEnemyState::Chasing);
            return;
        }

        m_attackCooldownRemaining = std::max(0.0f, m_attackCooldownRemaining - deltaTime);

        const aiko::vec2 playerDelta = playerPosition - m_position;
        const float distanceToPlayer = aiko::math::length(playerDelta);

        if (distanceToPlayer > Epsilon)
        {
            const aiko::vec2 directionToPlayer = aiko::math::normalize(playerDelta);
            m_direction = convertVectorToDirection(directionToPlayer);
        }

        if (getAttackFrameCount() == 0)
        {
            m_animation = WolfEnemyAnimation::Idle;
            return;
        }

        if (m_attackCooldownRemaining <= 0.0f && m_animation != WolfEnemyAnimation::Attack)
        {
            m_animation = WolfEnemyAnimation::Attack;
            m_animationFrame = 0;
            m_animationTime = 0.0f;
            m_attackCooldownRemaining = 1.0f;
        }
    }

    void WolfEnemy::updateChasing(float deltaTime, WolfMap& map, const aiko::vec2& playerPosition)
    {
        constexpr float ChaseSpeed = 1.0f;
        constexpr float Epsilon = 0.001f;

        if (canAttackPlayer(map, playerPosition))
        {
            transitionTo(WolfEnemyState::Attacking);
            return;
        }

        const aiko::vec2 playerDelta = playerPosition - m_position;
        const float distanceToPlayer = aiko::math::length(playerDelta);

        if (distanceToPlayer <= Epsilon)
        {
            m_animation = WolfEnemyAnimation::Idle;
            return;
        }

        const aiko::vec2 directionToPlayer = aiko::math::normalize(playerDelta);
        m_direction = convertVectorToDirection(directionToPlayer);

        const WolfRayHit hit = WolfRaycaster::castRay(map, m_position, directionToPlayer, WolfRaycastMode::Render);

        const bool hasLineOfSight = hit.hit == false || hit.distance >= distanceToPlayer - Epsilon;
        const float attackRange = m_type == WolfEnemyType::Dog ? AttackRangeDog : AttackRangeEnemy;
        const float maxMovement = hasLineOfSight ? std::max(0.0f, distanceToPlayer - attackRange) : distanceToPlayer;
        const float movementDistance = std::min(ChaseSpeed * deltaTime, maxMovement);

        if (movementDistance <= 0.0f)
        {
            m_animation = WolfEnemyAnimation::Idle;
            return;
        }

        const aiko::vec2 previousPosition = m_position;
        const aiko::vec2 displacement = directionToPlayer * movementDistance;

        const aiko::vec2 candidatePositionX = m_position + aiko::vec2(displacement.x, 0.0f);
        if (map.collidesAt(candidatePositionX, EnemyRadius) == false)
        {
            m_position.x = candidatePositionX.x;
        }

        const aiko::vec2 candidatePositionY = m_position + aiko::vec2(0.0f, displacement.y);
        if (map.collidesAt(candidatePositionY, EnemyRadius) == false)
        {
            m_position.y = candidatePositionY.y;
        }

        const float movedDistanceSquared = aiko::math::distanceSquared(previousPosition, m_position);
        m_animation = movedDistanceSquared > Epsilon * Epsilon ? WolfEnemyAnimation::Walk : WolfEnemyAnimation::Idle;
    }

    bool WolfEnemy::tryDetectPlayer(const WolfMap& map, const aiko::vec2& playerPosition)
    {
        if (canSeePlayer(map, playerPosition) == false)
        {
            return false;
        }
        transitionTo(WolfEnemyState::Chasing);
        return true;
    }

    bool WolfEnemy::canSeePlayer(const WolfMap& map, const aiko::vec2& playerPosition) const
    {
        constexpr uint8_t MaxDetectionTiles = 8;
        constexpr float ApproxToZero = 0.001f;
        const float FieldOfView = aiko::math::radians(120.0f);

        const aiko::vec2 playerDelta = playerPosition - m_position;
        const float distanceToPlayer = aiko::math::length(playerDelta);

        if (distanceToPlayer > MaxDetectionTiles)
        {
            return false;
        }

        if (distanceToPlayer <= ApproxToZero)
        {
            return true;
        }

        const aiko::vec2 directionToPlayer = aiko::math::normalize(playerDelta);

        if (distanceToPlayer > 1.5f)
        {
            const aiko::vec2 directionVector = convertDirectionToVector(m_direction);
            const aiko::vec2 facing = aiko::math::normalize(directionVector);
            const auto facingDot = aiko::math::dot(facing, directionToPlayer);
            if (facingDot < aiko::math::cos(FieldOfView / 2.0f))
            {
                return false;
            }
        }

        const WolfRayHit hit = WolfRaycaster::castRay(map, m_position, directionToPlayer, WolfRaycastMode::Render);

        if (hit.hit == false)
        {
            return true;
        }
        if(hit.distance >= distanceToPlayer)
        {
            return true;
        }

        return false;
    }

    bool WolfEnemy::canAttackPlayer(const WolfMap& map, const aiko::vec2& playerPosition) const
    {
        constexpr float Epsilon = 0.001f;

        const aiko::vec2 playerDelta = playerPosition - m_position;
        const float distanceToPlayer = aiko::math::length(playerDelta);

        const float attackRange = m_type == WolfEnemyType::Dog ? AttackRangeDog : AttackRangeEnemy;
        if (distanceToPlayer > attackRange)
        {
            return false;
        }

        if (distanceToPlayer <= Epsilon)
        {
            return true;
        }

        const aiko::vec2 directionToPlayer = aiko::math::normalize(playerDelta);
        const WolfRayHit hit = WolfRaycaster::castRay(map, m_position, directionToPlayer, WolfRaycastMode::Render);
        return !hit.hit || hit.distance >= distanceToPlayer - Epsilon;
    }

    uint8_t WolfEnemy::getAttackFrameCount() const
    {
        switch (m_type)
        {
            case WolfEnemyType::Guard:
            case WolfEnemyType::Dog:
            case WolfEnemyType::SS:
            case WolfEnemyType::Officer:
                return 3;
            case WolfEnemyType::Mutant:
                return 4;
            default:
                return 0;
        }
    }

    WolfEnemy WolfEnemy::fromSpawn(const WolfEnemySpawn& spawn)
    {
        aiko::vec2 position = aiko::vec2(spawn.tile.x + 0.5f, spawn.tile.y + 0.5f);
        return WolfEnemy(position, spawn.type, spawn.direction, spawn.spawnMode);
    }
}
