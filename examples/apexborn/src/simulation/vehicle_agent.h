#pragma once

#include "vehicle.h"
#include "driving_observation.h"
#include "road_sensors.h"

namespace ab
{

    struct VehicleAgent
    {
        Vehicle car;
        bool alive = true;

        float previousProgress = 0.0f;
        float totalProgress = 0.0f;

        float bestProgress = 0.0f;

        float referenceSteeringBias = 0.0f;
        float referenceThrottleScale = 1.0f;

        RoadSensors::Readings sensors{};
        DrivingObservationData observation{};

    };

}