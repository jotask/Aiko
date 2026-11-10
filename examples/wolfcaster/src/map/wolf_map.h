#pragma once

#include "wolf_door_system.h"
#include "wolf_map_types.h"

#include <array>
#include <cstdint>
#include <vector>
#include <string>

namespace wc
{

    class WolfMap
    {
    public:

        explicit WolfMap(WolfMapData data);

        void update(float deltaTime);

        uint16_t getWidth() const { return m_data.level.width; }
        uint16_t getHeight() const { return m_data.level.height; }
        uint16_t getLevelIndex() const { return m_data.level.index; }
        const std::string& getName() const { return m_data.level.name; }

        TileId getGeometryTile(uint16_t x, uint16_t y) const;

        const WolfPlayerSpawn& getPlayerSpawn() const { return m_data.playerSpawn; }
        const std::vector<WolfWorldObject>& getObjects() const { return m_data.objects; }

        bool canInteract(int32_t tileX, int32_t tileY) const;
        bool interact(int32_t tileX, int32_t tileY);

        bool isBlockingTile(int32_t x, int32_t y) const;
        bool collidesAt(const aiko::vec2& position, float radius) const;

        const WolfDoor* getDoor(int32_t tileX, int32_t tileY) const;

    private:

        size_t getTileIndex(uint16_t x, uint16_t y) const;

        WolfMapData m_data;
        WolfDoorSystem m_doorSystem;

    };

}