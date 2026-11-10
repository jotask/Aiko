#include "wolf_map_loader.h"

#include "aiko_macros.h"
#include "byte_utils.h"
#include "carmack_decoder.h"
#include "core/file.h"
#include "logger/logger.h"
#include "rlew_decoder.h"
#include "wolf_map.h"

#include <optional>
#include <utility>

namespace wc
{

    namespace
    {
        constexpr const char* MapHead = "MAPHEAD.WL1";
        constexpr const char* GameMaps = "GAMEMAPS.WL1";
    }

    WolfMap WolfMapLoader::loadLevel(const std::filesystem::path& dataDirectory, uint16_t levelIndex)
    {

        const std::filesystem::path mapHeadPath = dataDirectory / MapHead;
        const std::filesystem::path gameMapsPath = dataDirectory / GameMaps;

        AIKO_ASSERT(std::filesystem::exists(mapHeadPath), "MAPHEAD don't exist");
        AIKO_ASSERT(std::filesystem::exists(gameMapsPath), "GAMEMAPS don't exist");

        const std::vector<std::uint8_t> mapHeadBytes = aiko::files::readFileBytes(mapHeadPath);
        AIKO_ASSERT(mapHeadBytes.size() >= 2, "Invalid MAPHEAD");

        const uint16_t rlewTag = readU16LE(mapHeadBytes, 0);
        const size_t offsetPosition = 2 + size_t(levelIndex) * 4;
        AIKO_ASSERT(offsetPosition <= mapHeadBytes.size() && mapHeadBytes.size() - offsetPosition >= 4, "Invalid level index");
        const uint32_t levelOffset = readU32LE(mapHeadBytes, offsetPosition);
        AIKO_ASSERT(levelOffset != 0 && levelOffset != UINT32_MAX, "Level is unavailable");

        aiko::logger::Log::infoFmt("RLEW Tag: 0x{:04X}", rlewTag);
        aiko::logger::Log::infoFmt("Level {} Offset: {} (0x{:08X})", levelIndex, levelOffset, levelOffset);

        const std::vector<std::uint8_t> gameMapBytes = aiko::files::readFileBytes(gameMapsPath);
        AIKO_ASSERT(levelOffset <= gameMapBytes.size() && gameMapBytes.size() - levelOffset >= 38, "Invalid level header offset");

        const WolfLevelHeader levelInfo = readLevelHeader(gameMapBytes, levelOffset);
        AIKO_ASSERT(levelInfo.width > 0 && levelInfo.height > 0, "Invalid level dimensions");

        const size_t expectedTileCount = size_t(levelInfo.width) * size_t(levelInfo.height);

        std::vector<TileId> geometry = decodePlane(gameMapBytes, levelInfo.planesOffset[0].first, levelInfo.planesOffset[0].second, rlewTag, expectedTileCount);
        std::vector<TileId> objectPlane = decodePlane(gameMapBytes, levelInfo.planesOffset[1].first, levelInfo.planesOffset[1].second, rlewTag, expectedTileCount);
        std::vector<TileId> info;
        if (levelInfo.planesOffset[2].second > 0)
        {
            info = decodePlane(gameMapBytes, levelInfo.planesOffset[2].first, levelInfo.planesOffset[2].second, rlewTag, expectedTileCount);
        }
        else
        {
            info.resize(expectedTileCount, 0);
        }

        // Player Spawn && Objects
        std::optional<WolfPlayerSpawn> playerSpawn;
        std::vector<WolfWorldObject> objects;
        std::vector<WolfEnemySpawn> enemySpawns;
        for (size_t i = 0 ; i < objectPlane.size(); ++i)
        {
            const TileId objectId = objectPlane[i];

            const Tile tile = {
                static_cast<uint16_t>(i % levelInfo.width),
                static_cast<uint16_t>(i / levelInfo.width),
            };

            if (objectId >= 19 && objectId <= 22)
            {
                // Player Spawn
                AIKO_ASSERT(!playerSpawn.has_value(), "Multiple player spawns found");
                playerSpawn = WolfPlayerSpawn{
                    .tile = tile,
                    .direction = static_cast<WolfDirection>(objectId)
                };
                continue;
            }

            // Static object
            if (objectId >= 23 && objectId <= 70)
            {
                objects.push_back({
                    .position = {
                        static_cast<float>(tile.x) + 0.5f,
                        static_cast<float>(tile.y) + 0.5f
                    },
                    .mapObjectId = objectId,
                    .spriteIndex = size_t(objectId - 23) + 2,
                    .blocking = isBlockingStaticObject(objectId)
                });
                continue;
            }

            // Enemy Spawn
            if (objectId >= 108 && objectId <= 259)
            {
                std::optional<WolfEnemySpawn> enemySpawn = parseEnemySpawn(objectId, tile);
                if (enemySpawn != std::nullopt)
                {
                    enemySpawns.push_back(*enemySpawn);
                }
                continue;
            }

        }

        AIKO_ASSERT(playerSpawn.has_value(), "Player spawn not found");

        WolfMapData data{
            .level = {
                .index = levelIndex,
                .name = levelInfo.name,
                .width = levelInfo.width,
                .height = levelInfo.height
            },
            .geometry = std::move(geometry),
            .info = std::move(info),
            .playerSpawn = *playerSpawn,
            .objects = std::move(objects),
            .enemySpawns = std::move(enemySpawns),
        };

        return WolfMap(std::move(data));

    }

    std::vector<WolfLevelInfo> WolfMapLoader::getAvailableLevels(const std::filesystem::path& dataDirectory)
    {
        const std::filesystem::path mapHeadPath = dataDirectory / MapHead;
        const std::filesystem::path gameMapsPath = dataDirectory / GameMaps;

        AIKO_ASSERT(std::filesystem::exists(mapHeadPath), "MAPHEAD doesn't exist");
        AIKO_ASSERT(std::filesystem::exists(gameMapsPath), "GAMEMAPS doesn't exist");

        const std::vector<uint8_t> mapHeadBytes = aiko::files::readFileBytes(mapHeadPath);
        const std::vector<uint8_t> gameMapBytes = aiko::files::readFileBytes(gameMapsPath);

        AIKO_ASSERT(mapHeadBytes.size() >= 2, "Invalid MAPHEAD");

        const size_t levelCount = (mapHeadBytes.size() - 2) / 4;

        std::vector<WolfLevelInfo> availableLevels;

        for (size_t i = 0; i < levelCount && i <= UINT16_MAX; ++i)
        {
            const uint32_t levelOffset = readU32LE(mapHeadBytes, 2 + i * 4);

            if (levelOffset == 0 || levelOffset == UINT32_MAX)
            {
                continue;
            }

            if (levelOffset > gameMapBytes.size() ||
                gameMapBytes.size() - levelOffset < 38)
            {
                continue;
            }

            const uint16_t width = readU16LE(gameMapBytes, levelOffset + 18);
            const uint16_t height = readU16LE(gameMapBytes, levelOffset + 20);

            if (width == 0 || height == 0)
            {
                continue;
            }

            bool valid = true;

            for (size_t plane = 0; plane < 3; ++plane)
            {
                const uint32_t offset = readU32LE(gameMapBytes, levelOffset + plane * 4);
                const uint16_t length = readU16LE(gameMapBytes, levelOffset + 12 + plane * 2);
                if (plane == 2 && length == 0)
                {
                    continue;
                }
                if (length == 0 || offset > gameMapBytes.size() || gameMapBytes.size() - offset < length)
                {
                    valid = false;
                    break;
                }
            }
            if (valid)
            {

                const WolfLevelHeader header = readLevelHeader(gameMapBytes, levelOffset);

                availableLevels.push_back({
                    .index = static_cast<uint16_t>(i),
                    .name = header.name,
                    .width = header.width,
                    .height = header.height
                });
            }
        }

        return availableLevels;

    }

    WolfMapLoader::WolfLevelHeader WolfMapLoader::readLevelHeader(std::span<const uint8_t> data, size_t offset)
    {
        WolfLevelHeader info;

        info.planesOffset[0] = {readU32LE(data, offset + 0), readU16LE(data, offset + 12)};
        info.planesOffset[1] = {readU32LE(data, offset + 4), readU16LE(data, offset + 14)};
        info.planesOffset[2] = {readU32LE(data, offset + 8), readU16LE(data, offset + 16)};

        info.width = readU16LE(data, offset + 18);
        info.height = readU16LE(data, offset + 20);

        std::vector<uint8_t> name;
        for (size_t i = 0 ; i < 16 ; ++i)
        {
            uint8_t byte = data[offset + 22 + i];
            if (byte == 0x00)
            {
                break;
            }
            name.push_back(byte);
        }

        info.name = std::string(name.begin(), name.end());

        aiko::logger::Log::infoFmt("Level Header Info");
        aiko::logger::Log::infoFmt("     Name: {}", info.name);
        aiko::logger::Log::infoFmt("     Width: {} Height: {}", info.width, info.height);
        for (size_t i = 0 ; i < info.planesOffset.size(); ++i)
        {
            aiko::logger::Log::infoFmt("     Plane {}: Offset {} CompressedLength: {} ", i, info.planesOffset[i].first, info.planesOffset[i].second);
        }

        return info;
    }

    std::vector<uint16_t> WolfMapLoader::decodePlane(std::span<const uint8_t> gameMapBytes, uint32_t offset, uint16_t compressedLength, uint16_t rlewTag, size_t expectedTileCount)
    {
        AIKO_ASSERT(offset <= gameMapBytes.size() && compressedLength <= gameMapBytes.size() - offset, "Plane data exceeds GAMEMAPS file boundaries");
        std::span<const uint8_t> plane = {gameMapBytes.begin() + offset, compressedLength};
        const std::vector<uint16_t> decodedCarmackPlane = CarmackDecoder::decode(plane);
        std::vector<uint16_t> decodedRlewPlane = RlewDecoder::decode(decodedCarmackPlane, rlewTag);
        AIKO_ASSERT(decodedRlewPlane.size() == expectedTileCount, "Decoded plane size does not match map dimensions");
        return decodedRlewPlane;
    }

    std::optional<WolfEnemySpawn> WolfMapLoader::parseEnemySpawn(TileId objectId, Tile tile)
    {
        auto parseEnemyDirection = [](TileId objectId, TileId groupStart) -> WolfDirection
        {
            if (objectId == groupStart + 0) return WolfDirection::East;
            if (objectId == groupStart + 1) return WolfDirection::North;
            if (objectId == groupStart + 2) return WolfDirection::West;
            if (objectId == groupStart + 3) return WolfDirection::South;
            AIKO_ASSERT(false, "Unknown direction");
        };

        WolfEnemySpawn spawn;
        spawn.tile = tile;
        spawn.mapObjectId = objectId;

        const auto tryParseEnemy = [&](TileId firstId, WolfEnemyType enemyType, WolfEnemySpawnMode mode, WolfDifficulty difficulty) -> bool
        {
            if (objectId >= firstId && objectId <= firstId + 3)
            {
                spawn.type = enemyType;
                spawn.spawnMode = mode;
                spawn.minDifficulty = difficulty;
                spawn.direction = parseEnemyDirection(objectId, firstId);
                return true;
            }
            return false;
        };

        const auto tryParseSpecialEnemy = [&](TileId tileId, WolfEnemyType enemyType, WolfDirection direction) -> bool
        {
            if (objectId == tileId)
            {
                spawn.type = enemyType;
                spawn.direction = direction;
                spawn.spawnMode = WolfEnemySpawnMode::Special;
                return true;
            }
            return false;
        };

        // Guards
        if (tryParseEnemy(108, WolfEnemyType::Guard, WolfEnemySpawnMode::Standing, WolfDifficulty::Baby) == true) { return spawn; }
        if (tryParseEnemy(144, WolfEnemyType::Guard, WolfEnemySpawnMode::Standing, WolfDifficulty::Medium) == true) { return spawn; }
        if (tryParseEnemy(180, WolfEnemyType::Guard, WolfEnemySpawnMode::Standing, WolfDifficulty::Hard) == true) { return spawn; }
        if (tryParseEnemy(112, WolfEnemyType::Guard, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Baby) == true) { return spawn; }
        if (tryParseEnemy(148, WolfEnemyType::Guard, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Medium) == true) { return spawn; }
        if (tryParseEnemy(184, WolfEnemyType::Guard, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Hard) == true) { return spawn; }

        // Officer
        if (tryParseEnemy(116, WolfEnemyType::Officer, WolfEnemySpawnMode::Standing, WolfDifficulty::Baby) == true) { return spawn; }
        if (tryParseEnemy(152, WolfEnemyType::Officer, WolfEnemySpawnMode::Standing, WolfDifficulty::Medium) == true) { return spawn; }
        if (tryParseEnemy(188, WolfEnemyType::Officer, WolfEnemySpawnMode::Standing, WolfDifficulty::Hard) == true) { return spawn; }
        if (tryParseEnemy(120, WolfEnemyType::Officer, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Baby) == true) { return spawn; }
        if (tryParseEnemy(156, WolfEnemyType::Officer, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Medium) == true) { return spawn; }
        if (tryParseEnemy(192, WolfEnemyType::Officer, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Hard) == true) { return spawn; }

        // SS
        if (tryParseEnemy(126, WolfEnemyType::SS, WolfEnemySpawnMode::Standing, WolfDifficulty::Baby) == true) { return spawn; }
        if (tryParseEnemy(162, WolfEnemyType::SS, WolfEnemySpawnMode::Standing, WolfDifficulty::Medium) == true) { return spawn; }
        if (tryParseEnemy(198, WolfEnemyType::SS, WolfEnemySpawnMode::Standing, WolfDifficulty::Hard) == true) { return spawn; }
        if (tryParseEnemy(130, WolfEnemyType::SS, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Baby) == true) { return spawn; }
        if (tryParseEnemy(166, WolfEnemyType::SS, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Medium) == true) { return spawn; }
        if (tryParseEnemy(202, WolfEnemyType::SS, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Hard) == true) { return spawn; }

        // Dog
        if (tryParseEnemy(134, WolfEnemyType::Dog, WolfEnemySpawnMode::Standing, WolfDifficulty::Baby) == true) { return spawn; }
        if (tryParseEnemy(170, WolfEnemyType::Dog, WolfEnemySpawnMode::Standing, WolfDifficulty::Medium) == true) { return spawn; }
        if (tryParseEnemy(206, WolfEnemyType::Dog, WolfEnemySpawnMode::Standing, WolfDifficulty::Hard) == true) { return spawn; }
        if (tryParseEnemy(138, WolfEnemyType::Dog, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Baby) == true) { return spawn; }
        if (tryParseEnemy(174, WolfEnemyType::Dog, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Medium) == true) { return spawn; }
        if (tryParseEnemy(210, WolfEnemyType::Dog, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Hard) == true) { return spawn; }

        // Mutant
        if (tryParseEnemy(216, WolfEnemyType::Mutant, WolfEnemySpawnMode::Standing, WolfDifficulty::Baby) == true) { return spawn; }
        if (tryParseEnemy(234, WolfEnemyType::Mutant, WolfEnemySpawnMode::Standing, WolfDifficulty::Medium) == true) { return spawn; }
        if (tryParseEnemy(252, WolfEnemyType::Mutant, WolfEnemySpawnMode::Standing, WolfDifficulty::Hard) == true) { return spawn; }
        if (tryParseEnemy(220, WolfEnemyType::Mutant, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Baby) == true) { return spawn; }
        if (tryParseEnemy(238, WolfEnemyType::Mutant, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Medium) == true) { return spawn; }
        if (tryParseEnemy(256, WolfEnemyType::Mutant, WolfEnemySpawnMode::Patrolling, WolfDifficulty::Hard) == true) { return spawn; }

        // Bosses and Ghosts
        if (tryParseSpecialEnemy(160, WolfEnemyType::FakeHitler, WolfDirection::North) == true) { return spawn; }
        if (tryParseSpecialEnemy(178, WolfEnemyType::MechaHitler, WolfDirection::South) == true) { return spawn; }
        if (tryParseSpecialEnemy(179, WolfEnemyType::GeneralFettgesicht, WolfDirection::South) == true) { return spawn; }
        if (tryParseSpecialEnemy(196, WolfEnemyType::DrSchabbs, WolfDirection::South) == true) { return spawn; }
        if (tryParseSpecialEnemy(197, WolfEnemyType::GretelGrosse, WolfDirection::North) == true) { return spawn; }
        if (tryParseSpecialEnemy(214, WolfEnemyType::HansGrosse, WolfDirection::South) == true) { return spawn; }
        if (tryParseSpecialEnemy(215, WolfEnemyType::OttoGiftmacher, WolfDirection::North) == true) { return spawn; }
        if (tryParseSpecialEnemy(224, WolfEnemyType::Blinky, WolfDirection::East) == true) { return spawn; }
        if (tryParseSpecialEnemy(225, WolfEnemyType::Clyde, WolfDirection::East) == true) { return spawn; }
        if (tryParseSpecialEnemy(226, WolfEnemyType::Pinky, WolfDirection::East) == true) { return spawn; }
        if (tryParseSpecialEnemy(227, WolfEnemyType::Inky, WolfDirection::East) == true) { return spawn; }

        return std::nullopt;
    }

    bool WolfMapLoader::isBlockingStaticObject(TileId objectId)
    {
        switch (objectId)
        {
            case 24: //	Green barrel
            case 25: //	Table and chairs
            case 26: //	Floor lamp
            case 28: //	Hanged man
            case 30: //	Red pillar
            case 31: //	Tree
            case 33: //	Sink
            case 34: //	Potted plant
            case 35: //	Urn
            case 36: //	Bare table
            case 39: //	Suit of armor
            case 40: //	Hanging cage
            case 41: //	Skeleton in cage
            case 45: //	Decorative object (stuff)
            case 58: //	Barrel
            case 59: //	Well
            case 60: //	Empty well
            case 62: //	Flag
            case 63: //	Call Apogee decoration
            case 68: //	Stove
            case 69: //	Spears
                return true;
        }
        return false;
    }
}
