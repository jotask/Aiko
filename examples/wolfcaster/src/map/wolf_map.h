#pragma once

#include "wolf_door_system.h"
#include "wolf_enemy_system.h"
#include "wolf_map_types.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace wc
{

    class WolfMap
    {
    public:

        explicit WolfMap(WolfMapData data);

        void update(float deltaTime, const aiko::vec2& playerPosition);

        uint16_t getWidth() const { return m_data.level.width; }
        uint16_t getHeight() const { return m_data.level.height; }
        uint16_t getLevelIndex() const { return m_data.level.index; }
        const std::string& getName() const { return m_data.level.name; }

        TileId getGeometryTile(uint16_t x, uint16_t y) const;
        TileId getObjectTile(uint16_t x, uint16_t y) const;

        const WolfPlayerSpawn& getPlayerSpawn() const { return m_data.playerSpawn; }
        const std::vector<WolfEnemySpawn>& getEnemySpawns() const { return m_data.enemySpawns; }
        const std::vector<WolfEnemy>& getEnemies() const { return m_enemySystem.getEnemies(); }
        const std::vector<WolfWorldObject>& getObjects() const { return m_data.objects; }

        void removeObject(size_t objectIndex);

        WolfEnemy& getEnemy(size_t enemyId);

        bool canInteract(int32_t tileX, int32_t tileY) const;
        bool interact(int32_t tileX, int32_t tileY, const aiko::ivec2& pushDirection, const aiko::vec2& playerPosition, uint8_t ownedKeys);

        bool isPushWall(int32_t x, int32_t y) const;
        bool isBlockingTile(int32_t x, int32_t y) const;
        bool collidesAt(const aiko::vec2& position, float radius) const;

        bool isExitSwitch(int32_t tileX, int32_t tileY) const;

        const WolfDoor* getDoor(int32_t tileX, int32_t tileY) const;
        bool requestDoorOpen(int32_t tileX, int32_t tileY);

        void setGeometryTile(uint16_t x, uint16_t y, TileId tileId);
        void setObjectTile(uint16_t x, uint16_t y, TileId tileId);

        const WolfDoor* getActivePushWall() const { return m_doorSystem.getActivePushWall(); }

    private:

        size_t getTileIndex(uint16_t x, uint16_t y) const;

        WolfMapData m_data;
        WolfDoorSystem m_doorSystem;
        WolfEnemySystem m_enemySystem;

    };

}