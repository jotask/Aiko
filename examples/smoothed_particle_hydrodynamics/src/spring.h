#pragma once

#include <cstddef>

namespace sph
{

    struct Spring
    {
    public:
        size_t particleA;
        size_t particleB;
        float length = 0.0f;
    };

}
