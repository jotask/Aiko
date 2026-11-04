#include "reference_driver.h"

#include "constant_types.h"
#include "vehicle.h"
#include "world/track.h"

#include <intrumentor/profiler.h>

#include <algorithm>
#include <cmath>

namespace ab
{
    VehicleControl ReferenceDriver::compute(const Vehicle& vehicle, const Track& track)
    {
        AIKO_FUNCTION_PROFILE
        const auto projection = track.projectPoint(vehicle.position());

        constexpr float LookAheadDistance = 1.5f;
        constexpr float SteeringGain = 1.5f;
        constexpr float PlanningDistance = 5.0f;
        constexpr float PlanningStep = 0.5f;
        constexpr float PlanningDeceleration = 1.0f;

        float targetSpeed = MaxTargetSpeed;
        constexpr size_t NSteps = PlanningDistance / PlanningStep;
        for (size_t i = 0 ; i <= NSteps; ++i)
        {
            const float offset = PlanningStep * i;
            const auto futureDistance =  projection.progressOnTrack + offset;
            const auto cornerSpeed = calculateSpeedLimitAtDistance(track, futureDistance);
            const auto velocityAllowed = std::sqrt((cornerSpeed * cornerSpeed) + 2 * PlanningDeceleration * offset);
            targetSpeed = std::min(targetSpeed, velocityAllowed);
        }

        // Steering
        const auto target = track.pointAtDistance(projection.progressOnTrack + LookAheadDistance);
        const auto direction = target - vehicle.position();
        const float desiredHeading = std::atan2(direction.z, direction.x);
        const float headingDifference = desiredHeading - vehicle.heading();
        const float headingError = std::atan2(std::sin(headingDifference), std::cos(headingDifference));
        const float steering =  aiko::math::clamp(headingError * SteeringGain, -1.0f, 1.0f);

        // Throttle
        constexpr float speedGain = 1.0f;
        constexpr float throttleBase = 0.133f;
        const float speedError = targetSpeed - vehicle.speed();
        const auto throttle = aiko::math::clamp(throttleBase + ( speedGain * speedError ), 0.0f, 1.0f);

        // brake
        constexpr float BrakeTolerance = 0.1f;
        constexpr float BrakeGain = 0.5f;

        const auto excessSpeed = vehicle.speed() - ( targetSpeed + BrakeTolerance );
        const auto brake = aiko::math::clamp(BrakeGain * excessSpeed, 0.0f, 1.0f);

        VehicleControl control;
        control.steering = steering;
        control.brake = brake;
        control.throttle = brake > 0.0f ? 0.0f : throttle;

        return control;

    }

    float ReferenceDriver::calculateSpeedLimitAtDistance(const Track& track, float distance)
    {
        constexpr float FutureVisionDistance = 1.5f;
        const auto pointAtDistanceA = track.pointAtDistance(distance + (0 * FutureVisionDistance));
        const auto pointAtDistanceB = track.pointAtDistance(distance + (1 * FutureVisionDistance));
        const auto pointAtDistanceC = track.pointAtDistance(distance + (2 * FutureVisionDistance));

        constexpr aiko::vec3 HorizontalMask = {1.0f, 0.0f, 1.0f};

        auto segmentAB = (pointAtDistanceB - pointAtDistanceA) * HorizontalMask;
        auto segmentBC = (pointAtDistanceC - pointAtDistanceB) * HorizontalMask;

        const auto squaredLengthAB = aiko::math::lengthSquared(segmentAB);
        const auto squaredLengthBC = aiko::math::lengthSquared(segmentBC);

        constexpr float epsilon2 = epsilon * epsilon;

        float turnAngle = 0.0f;
        if (squaredLengthAB > epsilon2 && squaredLengthBC > epsilon2)
        {
            segmentAB = aiko::math::normalize(segmentAB);
            segmentBC = aiko::math::normalize(segmentBC);

            const auto dot = aiko::math::dot(segmentAB, segmentBC);
            turnAngle = std::acos(aiko::math::clamp(dot, -1.0f, 1.0f));
        }

        const float normalizedAngle = aiko::math::clamp(turnAngle / MaxTurnAngle, 0.0f, 1.0f);
        const float targetSpeed = MaxTargetSpeed + ( MinTargetSpeed - MaxTargetSpeed ) * normalizedAngle;

        return targetSpeed;
    }
}
