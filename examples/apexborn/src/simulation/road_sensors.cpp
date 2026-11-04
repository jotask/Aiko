
#include "road_sensors.h"

#include "vehicle.h"
#include "world/track.h"

#include <intrumentor/profiler.h>

#include <cmath>
#include <numbers>

namespace ab
{
    RoadSensors::Readings RoadSensors::sense(const Vehicle& vehicle, const Track& track)
    {
        AIKO_FUNCTION_PROFILE

        constexpr float Pi = std::numbers::pi_v<float>;

        constexpr std::array<float, SensorCount> angles = {
            -Pi * 0.5f,
            -Pi * 0.25f,
             0.0f,
             Pi * 0.25f,
             Pi * 0.5f
        };

        Readings readings{};

        const auto& origin = vehicle.position();
        const float heading = vehicle.heading();

        for (size_t i = 0; i < SensorCount; ++i)
        {
            const float angle = heading + angles[i];

            const aiko::vec3 direction = { std::cos(angle), 0.0f, std::sin(angle) };

            const auto hit = track.raycastBoundary(origin, direction, MaxDistance);

            auto& reading = readings[i];

            if (hit)
            {
                reading.hit = true;
                reading.normalizedDistance = hit->distance / MaxDistance;
                reading.endPoint = hit->position;
            }
            else
            {
                reading.hit = false;
                reading.normalizedDistance = 1.0f;
                reading.endPoint = origin + direction * MaxDistance;
            }
        }

        return readings;
    }
}
