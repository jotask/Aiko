#include "reference_driver.h"

#include "vehicle.h"
#include "world/track.h"

#include <algorithm>
#include <cmath>

namespace ab
{
    float ReferenceDriver::computeSteering(const Vehicle& vehicle, const Track& track)
    {
        const auto projection = track.projectPoint(vehicle.position());

        constexpr float LookAheadDistance = 1.5f;
        constexpr float SteeringGain = 1.5f;

        const auto target = track.pointAtDistance(projection.progressOnTrack + LookAheadDistance);

        const auto direction = target - vehicle.position();

        const float desiredHeading = std::atan2(direction.z, direction.x);

        const float headingDifference = desiredHeading - vehicle.heading();

        // Normalize angle to [-pi, pi].
        const float headingError = std::atan2(std::sin(headingDifference), std::cos(headingDifference));

        return std::clamp(headingError * SteeringGain, -1.0f, 1.0f);
    }
}