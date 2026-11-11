#pragma once

#include <math/math_vector.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace wc
{

    using TileId = uint16_t;
    using Tile = aiko::vec2def<uint16_t>;

    enum class WolfMapPlane : uint8_t
    {
        Geometry = 0,
        Objects = 1,
        Info = 2,
    };

    enum class WolfDirection
    {
        North,
        NorthEast,
        East,
        SouthEast,
        South,
        SouthWest,
        West,
        NorthWest,
    };

    struct WolfPlayerSpawn
    {
        Tile tile = {0};
        WolfDirection direction = WolfDirection::North;
    };

    struct WolfLevelInfo
    {
        uint16_t index = 0;
        std::string name;
        uint16_t width = 0;
        uint16_t height = 0;
    };

    struct WolfWorldObject
    {
        aiko::vec2 position = {0.0f};
        TileId mapObjectId = 0;
        size_t spriteIndex = 0;
        bool blocking = false;
    };


    enum class WolfEnemyType
    {
        Guard,
        Officer,
        SS,
        Dog,
        Mutant,
        FakeHitler,
        MechaHitler,
        GeneralFettgesicht,
        DrSchabbs,
        GretelGrosse,
        HansGrosse,
        OttoGiftmacher,
        Blinky,
        Clyde,
        Pinky,
        Inky,
    };

    enum class WolfEnemySpawnMode
    {
        Standing,
        Patrolling,
        Special
    };

    enum class WolfEnemyAnimation
    {
        Idle,
        Walk,
        Attack,
    };

    enum class WolfEnemyState : uint8_t
    {
        Standing,
        Patrolling,
        Chasing,
        Attacking,
    };

    enum class WolfDifficulty : uint8_t
    {
        Baby = 0,
        Easy = 1,
        Medium = 2,
        Hard = 3,
    };

    struct WolfEnemySpawn
    {
        Tile tile = {0};
        WolfEnemyType type = WolfEnemyType::Guard;
        WolfDirection direction = WolfDirection::North;
        WolfEnemySpawnMode spawnMode = WolfEnemySpawnMode::Standing;
        WolfDifficulty minDifficulty = WolfDifficulty::Baby;
        TileId mapObjectId = {0};
    };

    struct WolfMapData
    {
        WolfLevelInfo level;

        std::vector<TileId> geometry;
        std::vector<TileId> info;

        std::vector<TileId> objectPlane;

        WolfPlayerSpawn playerSpawn;
        std::vector<WolfWorldObject> objects;
        std::vector<WolfEnemySpawn> enemySpawns;
    };

}