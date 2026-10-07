#pragma once

#include <math/math_vector.h>

namespace lp
{

    enum class ParticleType
    {
        Red,
        Green,
        Blue
    };

    struct Particle
    {
        ParticleType type = ParticleType::Red;
        aiko::vec3 position;
        aiko::vec3 velocity;
    };

}
