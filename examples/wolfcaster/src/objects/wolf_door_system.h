#pragma once

#include "map/wolf_map.h"
#include "wolf_door.h"

#include <optional>
#include <vector>
#include <cstddef>

namespace wc
{
    class WolfDoorSystem
    {
    public:

        void initialize(const WolfMap& map);

        void update(float deltaTime);

        std::optional<size_t> findDoorIndex(int32_t tileX, int32_t tileY) const;

        const WolfDoor& getDoor(size_t doorIndex) const;

        bool isBlocking(int32_t tileX, int32_t tileY) const;

        void toggleDoor(size_t doorIndex);

    private:

        std::vector<WolfDoor> m_doors;

    };
}
