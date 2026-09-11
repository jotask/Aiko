#pragma once

#include <math/math_vector.h>

namespace lp
{

    enum class ParticleType
    {
        RED,
    };

    struct Particle
    {
        ParticleType type = ParticleType::RED;
        aiko::vec3 position;
        aiko::vec3 velocity;
    };

}
