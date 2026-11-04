
#pragma once

#include <math/math_vector.h>

#include <array>
#include <cstddef>

namespace ab
{
    class Vehicle;
    class Track;

    class RoadSensors final
    {
    public:

        static constexpr size_t SensorCount = 5;
        static constexpr float MaxDistance = 5.0f;

        struct Reading
        {
            float normalizedDistance = 1.0f;
            bool hit = false;
            aiko::vec3 endPoint = {};
        };

        using Readings = std::array<Reading, SensorCount>;

        static Readings sense(const Vehicle& vehicle, const Track& track);
    };
}
