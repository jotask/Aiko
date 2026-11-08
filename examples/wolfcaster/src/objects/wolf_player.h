#pragma once
#include "map/wolf_map_types.h"
#include "math/math_vector.h"

namespace wc
{
    class WolfMap;

    class WolfPlayer
    {
    public:

        void move(const aiko::vec2 displacement, const WolfMap& map);
        void rotate(float angle);

        const aiko::vec2& position() const { return m_position; }
        float angle() const { return m_angleRadians; }

        aiko::vec2 forward() const;
        aiko::vec2 right() const;

        static WolfPlayer fromSpawn(const WolfPlayerSpawn& spawn);

    private:

        static constexpr float m_radius = 0.2f;

        aiko::vec2 m_position = {0.0f};
        float m_angleRadians = 0.0f;

    };

}

