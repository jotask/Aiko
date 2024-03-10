#include "particle.h"


namespace life
{
    void Particle::update()
    {
        velocity += acceleration;
        position += velocity;
        acceleration = { 0.0f };
    }
}

