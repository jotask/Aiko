#pragma once
#include "math/math.h"
#include "vehicle_controls.h"

namespace ab
{
    class Track;
    class Vehicle;

    class ReferenceDriver final
    {
    public:
        static VehicleControl compute(const Vehicle& vehicle, const Track& track);
    private:
        static constexpr float MaxTargetSpeed = 2.5f;
        static constexpr float MinTargetSpeed = 0.8f;
        static constexpr float MaxTurnAngle = aiko::PI / 6; // 30 degree

        static float calculateSpeedLimitAtDistance(const Track& track, float distance);
    };
}