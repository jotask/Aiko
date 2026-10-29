#pragma once

#include "spline.h"

#include <cstdint>
#include <optional>

namespace ab
{

    struct TrackGeneratorSettings
    {
        uint32_t seed = 42;
        uint32_t controlPointCount = 25;

        aiko::vec2 areaSize = {50.0f, 40.0f};

        float minDistance = 2.0f;
        float maxDisplacement = 5.0f;
        float minControlPointDistance = 1.5f;
        float maxTurnAngle = 90.0f;

    };

    class TrackGenerator
    {
    public:

        struct TrackGenerationDebug
        {
            std::vector<aiko::vec2> randomPoints;
            std::vector<aiko::vec2> convexHull;
            std::vector<aiko::vec2> displacedPoints;
            std::vector<aiko::vec2> correctedPoints;
        };

        static std::optional<Spline::Points> generate(const TrackGeneratorSettings& settings, TrackGenerationDebug* debug = nullptr);
    private:

        static std::vector<aiko::vec2> generateRandomPoints(const TrackGeneratorSettings& settings, std::mt19937& rng);
        static std::vector<aiko::vec2> computeConvexHull(std::vector<aiko::vec2> points);
        static std::vector<aiko::vec2> displaceMidpoints(const std::vector<aiko::vec2>& hull, float maxDisplacement, std::mt19937& rng);
        static bool pushPointsApart(std::vector<aiko::vec2>& points, float minDistance);
        static bool fixAngles(std::vector<aiko::vec2>& points, float maxAngleDegrees);

        static bool validateAngles(const std::vector<aiko::vec2>& points, float maxAngleDegrees);

    };

}