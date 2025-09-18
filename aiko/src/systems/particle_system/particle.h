#pragma once

#include "shared/math.h"
#include "types/color.h"

namespace aiko
{

    class Particle
    {
        friend class ParticleEmitter;
    public:
        Particle();
        Particle(vec3,Color);
    protected:
        vec3 position;
        Color color;
    private:

    };

}
