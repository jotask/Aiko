#pragma once

#include "aiko_types.h"
#include "shared/math.h"

namespace sandbox
{

    class Particle
    {
    public:
        aiko::vec2 position;
        aiko::vec2 velocity;
        aiko::vec2 acceleration;

        void update();

    };

}

