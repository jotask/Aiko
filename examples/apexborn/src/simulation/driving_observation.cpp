#include "driving_observation.h"

#include "constant_types.h"
#include "vehicle.h"
#include "world/track.h"

#include <cmath>

namespace ab
{

    namespace
    {
        float calculateLookAheadError (const Track& track, const Vehicle& vehicle, float progressOnTrack, float distance)
        {
            const auto target = track.pointAtDistance(progressOnTrack + distance);

            const auto direction = target - vehicle.position();

            const float horizontalLengthSquared = direction.x * direction.x + direction.z * direction.z;

            if (horizontalLengthSquared <= epsilon * epsilon)
            {
                return 0.0f;
            }

            const float desiredHeading = std::atan2(direction.z, direction.x);

            const float difference = desiredHeading - vehicle.heading();

            const float angle = std::atan2(std::sin(difference), std::cos(difference));

            return aiko::math::clamp(angle / static_cast<float>(aiko::PI), -1.0f, 1.0f);
        };
    }

    DrivingObservationData DrivingObservation::calculate(const Vehicle& vehicle, const Track& track, const TrackNavigation::TrackProjection& projection, const RoadSensors::Readings& sensors)
    {
        constexpr float NearLookAheadDistance = 1.5f;
        constexpr float FarLookAheadDistance = 3.0f;
        constexpr float CurvatureSampleDistance = 1.5f;
        constexpr float CurvatureNormalizationAngle = aiko::PI / 6.0f;

        // 1. Lateral offset
        const float halfTrackWidth = track.getTrackWidth() * 0.5f;
        AIKO_ASSERT(halfTrackWidth > epsilon, "Invalid track width");

        const float lateralOffset = aiko::math::clamp(projection.signedLateralOffset / halfTrackWidth, -1.0f, 1.0f);

        // 2. Heading error
        const auto vehicleForward = vehicle.forwardDirection();
        const auto trackForward = projection.forwardDirection;
        const float cross = vehicleForward.x * trackForward.z - vehicleForward.z * trackForward.x;
        const float dot = vehicleForward.x * trackForward.x + vehicleForward.z * trackForward.z;
        const float headingError = std::atan2(cross, dot) / aiko::PI;

        // 3 & 4. Near and far look-ahead errors
        const float lookAheadError = calculateLookAheadError(track, vehicle, projection.progressOnTrack, NearLookAheadDistance);
        const float farLookAheadError = calculateLookAheadError(track, vehicle, projection.progressOnTrack, FarLookAheadDistance);

        // 5. Upcoming curvature
        const auto pointA = track.pointAtDistance(projection.progressOnTrack);
        const auto pointB = track.pointAtDistance(projection.progressOnTrack + CurvatureSampleDistance);
        const auto pointC = track.pointAtDistance(projection.progressOnTrack + CurvatureSampleDistance * 2.0f);

        constexpr aiko::vec3 HorizontalMask = {1.0f, 0.0f, 1.0f};

        auto segmentAB = (pointB - pointA) * HorizontalMask;
        auto segmentBC = (pointC - pointB) * HorizontalMask;

        const float squaredLengthAB = aiko::math::lengthSquared(segmentAB);
        const float squaredLengthBC = aiko::math::lengthSquared(segmentBC);

        float turnAngle = 0.0f;

        if (squaredLengthAB > epsilon * epsilon && squaredLengthBC > epsilon * epsilon)
        {
            segmentAB = aiko::math::normalize(segmentAB);
            segmentBC = aiko::math::normalize(segmentBC);
            const float directionDot = aiko::math::clamp(aiko::math::dot(segmentAB, segmentBC), -1.0f, 1.0f);
            turnAngle = std::acos(directionDot);
        }

        const float upcomingCurvature = aiko::math::clamp(turnAngle / CurvatureNormalizationAngle, 0.0f, 1.0f);

        // 6. Normalized speed
        const float normalizedSpeed = aiko::math::clamp(vehicle.speed() / Vehicle::MaxSpeed, 0.0f, 1.0f);

        DrivingObservationData data
        {
            .lateralOffset = lateralOffset,
            .headingError = headingError,
            .lookAheadError = lookAheadError,
            .farLookAheadError = farLookAheadError,
            .upcomingCurvature = upcomingCurvature,
            .normalizedSpeed = normalizedSpeed,
        };

        for (size_t i = 0; i < RoadSensors::SensorCount; ++i)
        {
            data.roadDistances[i] = sensors[i].normalizedDistance;
        }

        return data;
    }

    std::array<double, DrivingObservationData::TotalMembers> DrivingObservation::generateInputs(const DrivingObservationData& data)
    {
        std::array<double, DrivingObservationData::TotalMembers> inputs = {};

        size_t index = 0;

        inputs[index++] = data.lateralOffset;
        inputs[index++] = data.headingError;
        inputs[index++] = data.lookAheadError;
        inputs[index++] = data.farLookAheadError;
        inputs[index++] = data.upcomingCurvature;
        inputs[index++] = data.normalizedSpeed;

        for (size_t i = 0; i < RoadSensors::SensorCount; ++i)
        {
            inputs[index++] = data.roadDistances[i];
        }

        AIKO_ASSERT(index == inputs.size(), "Inputs generated are not the same as expected");

        return inputs;

    }

}
