#pragma once
#include "math/math_vector.h"
#include "types/color.h"

namespace sph
{

    struct SPHParticle
    {
        aiko::vec3 position = {};
        aiko::vec3 prevPosition = {};
        aiko::vec3 velocity = {};
        aiko::vec3 acceleration = {};
        aiko::Color color = aiko::RED;
        float density = 0.0f;
        float pressure = 0.0f;
    };

    struct SPHParameters
    {
        float particleRadius = 0.05f;
        float smoothingRadius = 0.2f;

        float restDensity = 1000.0f;
        float gasConstant = 2000.0f;
        float viscosity = 0.1f;

        float gravity = 9.81f;
        aiko::vec3 gravityDirection = { 0.0f, -1.0f, 0.0f};

        float fixedDeltaTime = 1.0f / 120.0f;
    };

    struct SimulationState
    {
        int nParticles;
        float mass;
    };

    struct WorldBounds
    {
        aiko::vec3 position;
        aiko::vec3 size;
    };

}
