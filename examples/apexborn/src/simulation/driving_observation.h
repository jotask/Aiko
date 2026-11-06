#pragma once

#include "road_sensors.h"
#include "world/track_navigation.h"

#include <array>

namespace ab
{

    class Vehicle;
    class Track;

    struct DrivingObservationData
    {
        float lateralOffset = 0.0f;
        float headingError = 0.0f;
        float lookAheadError = 0.0f;
        float farLookAheadError = 0.0f;
        float upcomingCurvature = 0.0f;
        float normalizedSpeed = 0.0f;

        std::array<float, RoadSensors::SensorCount> roadDistances{};

        static constexpr size_t TotalMembers = 6 + RoadSensors::SensorCount;

    };

    class DrivingObservation final
    {
    public:
        static DrivingObservationData calculate(const Vehicle& vehicle, const Track& track, const TrackNavigation::TrackProjection& projection, const RoadSensors::Readings& sensors);
        static std::array<double, DrivingObservationData::TotalMembers> generateInputs(const DrivingObservationData& data);
    };

}