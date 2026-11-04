#pragma once

namespace ab
{
    class Track;
    class Vehicle;

    class ReferenceDriver final
    {
    public:
        static float computeSteering(const Vehicle& vehicle, const Track& track);
    };
}