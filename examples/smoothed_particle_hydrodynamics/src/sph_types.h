#pragma once

#include "math/math_vector.h"
#include "types/color.h"

namespace sph
{

    static constexpr aiko::u64 N_PARTICLES = 1024;
    static constexpr uint32_t MaxSprings = 65536;

    struct SPHParticle
    {
        aiko::vec3 position = {};
        aiko::vec3 prevPosition = {};
        aiko::vec3 velocity = {};
        aiko::vec3 acceleration = {};
        aiko::Color color = aiko::BLUE;
        float density = 0.0f;
        float pressure = 0.0f;
    };

    struct SPHParameters
    {
        float particleRadius = 0.05f;
        float smoothingRadius = 0.25f;

        float restDensity = 10.0f;
        float gasConstant = 2000.0f;
        float viscosity = 0.1f;

        float pressureStiffness = 0.005f;
        float nearPressureStiffness = 0.03f;

        // viscosity
        float sigma = 0.5f;
        float beta = 0.0f;

        // plasticity
        float gamma = 0.3f;
        float plasticity = 1.0f;
        float springStiffness = 0.9f;

        // sticky parameters
        float maxStickiness = smoothingRadius;
        float kStick = 0.1f;

        float gravity = 0.01f;
        aiko::vec3 gravityDirection = { 0.0f, -1.0f, 0.0f};

        float fixedDeltaTime = 0.3f;
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
