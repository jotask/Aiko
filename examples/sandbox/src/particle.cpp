#include "particle.h"


namespace sandbox
{
    void Particle::update()
    {
        velocity += acceleration;
        position += velocity;
        acceleration = { 0.0f };
    }
}

