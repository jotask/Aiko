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

        void update(float deltaTime);

        std::optional<size_t> findDoorIndex(int32_t tileX, int32_t tileY) const;

        const WolfDoor& getDoor(size_t doorIndex) const;

        bool isBlocking(int32_t tileX, int32_t tileY) const;

        bool collidesAt(const aiko::vec2& position, float radius) const;

        void toggleDoor(size_t doorIndex);

    private:

        std::vector<WolfDoor> m_doors;

    };
}
