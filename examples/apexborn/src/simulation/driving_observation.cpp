#include "driving_observation.h"

#include "constant_types.h"
#include "vehicle.h"
#include "world/track.h"

#include <cmath>

namespace ab
{

    DrivingObservationData DrivingObservation::calculate(const Vehicle& vehicle, const Track& track)
    {

        const auto trackWidth = track.getTrackWidth();
        AIKO_ASSERT(trackWidth > epsilon, "Track to small?");
        const auto halfTrackWidth = trackWidth * 0.5f;
        AIKO_ASSERT(halfTrackWidth > epsilon, "Track to small?");

        const auto projection = track.projectPoint(vehicle.position());

        const auto lateralOffset = aiko::math::clamp(projection.signedLateralOffset / halfTrackWidth, -1.0f, 1.0f);

        const auto vehicleForward = vehicle.forwardDirection();
        const auto cross = (vehicleForward.x * projection.forwardDirection.z) - (vehicleForward.z * projection.forwardDirection.x);
        const auto dot   = (vehicleForward.x * projection.forwardDirection.x) + (vehicleForward.z * projection.forwardDirection.z);
        const auto angle = atan2(cross, dot);
        const auto headingError = angle / aiko::PI;


        constexpr float LookAheadDistance = 1.5f;
        const auto target = track.pointAtDistance(projection.progressOnTrack + LookAheadDistance);
        const auto direction = target - vehicle.position();
        const float desiredHeading = std::atan2(direction.z, direction.x);
        const float headingDifference = desiredHeading - vehicle.heading();
        const float angleTowardsLookAhead = std::atan2(std::sin(headingDifference), std::cos(headingDifference));
        const float lookAheadError = angleTowardsLookAhead / aiko::PI;

        DrivingObservationData data;
        data.lateralOffset = lateralOffset;
        data.lookAheadError = lookAheadError;
        data.headingError = headingError;
        return data;
    }
}
