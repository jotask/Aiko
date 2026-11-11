#pragma once

#include "objects/wolf_door.h"

#include <cstddef>
#include <optional>
#include <vector>

namespace wc
{
    class WolfMap;
    class WolfDoorSystem
    {
    public:

        void initialize(const WolfMap& map);

        void update(float deltaTime, WolfMap& map, const aiko::vec2& playerPosition);

        std::optional<size_t> findDoorIndex(int32_t tileX, int32_t tileY) const;

        const WolfDoor& getDoor(size_t doorIndex) const;

        bool isBlocking(int32_t tileX, int32_t tileY) const;
        bool collidesAt(const aiko::vec2& position, float radius) const;

        void toggleDoor(size_t doorIndex);
        void openDoor(size_t doorIndex);

        bool activatePushWall(int32_t tileX, int32_t tileY, const aiko::ivec2& direction, WolfMap& map, const aiko::vec2& playerPosition);
        const WolfDoor* getActivePushWall() const;

    private:

        std::optional<size_t> findPushWallIndex(int32_t tileX, int32_t tileY) const;
        bool canPushWallInto(const WolfMap& map, int32_t tileX, int32_t tileY, const aiko::vec2& playerPosition) const;
        void updatePushWall(WolfDoor& wall, float deltaTime, WolfMap& map, const aiko::vec2& playerPosition);

        std::vector<WolfDoor> m_doors;
        std::optional<size_t> m_activePushWallIndex;

    };
}
