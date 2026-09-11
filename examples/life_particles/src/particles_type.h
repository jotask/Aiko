#pragma once

#include <math/math_vector.h>

namespace lp
{

    constexpr uint64_t c_particles_amount = 1000;

    struct BoundingBox
    {
        aiko::vec3 position;
        aiko::vec3 size;
    };

}
