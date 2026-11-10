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

        WolfMap(uint16_t levelIdx, std::string name, uint16_t width, uint16_t height, std::array<std::vector<uint16_t>, 3>);

        void update(float deltaTime);

        uint16_t getWidth() const { return m_width; }
        uint16_t getHeight() const { return m_height; }
        uint16_t getTile(WolfMapPlane plane, uint16_t x, uint16_t y) const;

        WolfPlayerSpawn findPlayerSpawn() const;

        bool canInteract(int32_t tileX, int32_t tileY) const;
        bool interact(int32_t tileX, int32_t tileY);

        bool isBlockingTile(int32_t x, int32_t y) const;

        bool collidesAt(const aiko::vec2& position, float radius) const;

        const WolfDoor* getDoor(int32_t tileX, int32_t tileY) const;

        uint16_t getLevelIndex() const { return m_levelIndex; }
        const std::string& getName() const { return m_name; }

    private:

        size_t getTileIndex(uint16_t x, uint16_t y) const;

        uint16_t m_levelIndex;
        std::string m_name;

        uint16_t m_width;
        uint16_t m_height;

        std::array<std::vector<uint16_t>, 3> m_planes;

        WolfDoorSystem m_doorSystem;

    };

}