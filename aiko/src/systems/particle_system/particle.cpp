#include "particle.h"

namespace aiko
{

    Particle::Particle()
        : Particle( {0.0f}, { 255, 255, 255, 255 })
    {

    }

    Particle::Particle(vec3 position, Color color)
        : position (position)
        , color (color)
    {

    }

}
