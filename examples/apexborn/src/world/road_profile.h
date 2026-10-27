#pragma once

#include <math/math.h>
#include <vector>

namespace ab
{
    struct RoadProfile
    {
        std::vector<aiko::vec2> points = {
            {-0.5f,  0.0f},
            { 0.5f,  0.0f},
            { 0.5f, -0.15f},
            {-0.5f, -0.15f}
        };

        bool closed = true;
    };
}