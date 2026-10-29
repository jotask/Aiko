#include "track_generator.h"

#include "constant_types.h"

#include <algorithm>

namespace ab
{

    namespace
    {
        float crossOrientation(const aiko::vec2& o, const aiko::vec2& a, const aiko::vec2& b)
        {
            const auto oa = a - o;
            const auto ob = b - o;
            return oa.x * ob.y - oa.y * ob.x;
        }
    }

    std::optional<Spline::Points> TrackGenerator::generate(const TrackGeneratorSettings& settings, TrackGenerationDebug* debug)
    {
        constexpr uint32_t MaxGenerationAttempts = 64;

        if (debug)
        {
            *debug = {};
        }

        AIKO_ASSERT(settings.controlPointCount >= 4, "At least four points required");
        AIKO_ASSERT(settings.areaSize.x > 0.0f, "Invalid generation area width");
        AIKO_ASSERT(settings.areaSize.y > 0.0f, "Invalid generation area height");
        AIKO_ASSERT(settings.minDistance >= 0.0f, "Invalid minimum point distance");
        AIKO_ASSERT(aiko::math::isfinite(settings.maxDisplacement) && settings.maxDisplacement >= 0.0f, "Invalid max displacement");
        AIKO_ASSERT(aiko::math::isfinite(settings.minControlPointDistance) && settings.minControlPointDistance >= 0.0f, "Invalid control point distance");
        AIKO_ASSERT(aiko::math::isfinite(settings.maxTurnAngle) && settings.maxTurnAngle > 0.0f && settings.maxTurnAngle <= 180.0f, "Invalid maximum turn angle");

        std::mt19937 rng(settings.seed);
        for (size_t attempts = 0 ; attempts < MaxGenerationAttempts; ++attempts)
        {

            const auto points = generateRandomPoints(settings, rng);

            if (points.size() < settings.controlPointCount)
            {
                continue;
            }

            const auto convexHull = computeConvexHull(points);

            if (convexHull.size() < 4)
            {
                continue;
            }

            const auto displacedPoints = displaceMidpoints(convexHull, settings.maxDisplacement, rng);

            auto correctedPoints = displacedPoints;

            constexpr size_t CorrectionPasses = 3;

            bool valid = true;

            for (size_t pass = 0; pass < CorrectionPasses; ++pass)
            {
                if (fixAngles(correctedPoints, settings.maxTurnAngle) == false)
                {
                    valid = false;
                    break;
                }

                if (pushPointsApart(correctedPoints, settings.minControlPointDistance) == false)
                {
                    valid = false;
                    break;
                }
            }

            if (valid == false || validateAngles(correctedPoints, settings.maxTurnAngle) == false)
            {
                continue;
            }

            if (debug)
            {
                debug->randomPoints = points;
                debug->convexHull = convexHull;
                debug->displacedPoints = displacedPoints;
                debug->correctedPoints = correctedPoints;
            }

            std::vector<aiko::vec3> result = {};
            for (const auto& hull : correctedPoints)
            {
                result.push_back({hull.x, 0.0f, hull.y});
            }

            return result;

        }
        return std::nullopt;
    }

    std::vector<aiko::vec2> TrackGenerator::generateRandomPoints(const TrackGeneratorSettings& settings, std::mt19937& rng)
    {
        std::vector<aiko::vec2> points;
        points.reserve(settings.controlPointCount);

        const uint32_t MaxAttempts = static_cast<size_t>(settings.controlPointCount) * 16;
        uint32_t attempts = 0;

        std::uniform_real_distribution<float> distributionX(-settings.areaSize.x * 0.5f, settings.areaSize.x * 0.5f);
        std::uniform_real_distribution<float> distributionY(-settings.areaSize.y * 0.5f, settings.areaSize.y * 0.5f);

        const float minDistanceSquare = settings.minDistance * settings.minDistance;

        while (points.size() < settings.controlPointCount && attempts < MaxAttempts)
        {
            const float x = distributionX(rng);
            const float y = distributionY(rng);

            const aiko::vec2 candidate = {x, y};

            bool validCandidate = true;

            for (const auto& existing : points)
            {
                const float distanceSquared = aiko::math::distanceSquared(candidate, existing);
                if (distanceSquared < minDistanceSquare)
                {
                    validCandidate = false;
                    break;
                }
            }

            if (validCandidate == true)
            {
                points.push_back(candidate);
            }

            ++attempts;

        }
        return points;
    }

    std::vector<aiko::vec2> TrackGenerator::computeConvexHull(std::vector<aiko::vec2> points)
    {

        std::sort(points.begin(), points.end());
        points.erase(std::unique(points.begin(), points.end()), points.end());

        if (points.size() < 3)
        {
            return {};
        }

        std::vector<aiko::vec2> lower = {};
        for (const auto& B : points)
        {
            while (lower.size() >= 2)
            {
                const auto& O = lower[lower.size() - 2];
                const auto& A = lower.back();
                if (crossOrientation(O, A , B) > 0)
                {
                    break;
                }
                lower.pop_back();
            }
            lower.push_back(B);
        }

        std::vector<aiko::vec2> upper = {};
        for (auto it = points.rbegin(); it != points.rend() ; ++it)
        {
            const auto& B = *it;
            while (upper.size() >= 2)
            {
                const auto& O = upper[upper.size() - 2];
                const auto& A = upper.back();
                if (crossOrientation(O, A , B) > 0)
                {
                    break;
                }
                upper.pop_back();
            }
            upper.push_back(B);
        }

        lower.pop_back();
        upper.pop_back();

        std::vector<aiko::vec2> hull;
        hull.reserve(lower.size() + upper.size());
        std::move(lower.begin(), lower.end(), std::back_inserter(hull));
        std::move(upper.begin(), upper.end(), std::back_inserter(hull));

        return hull;

    }

    std::vector<aiko::vec2> TrackGenerator::displaceMidpoints(const std::vector<aiko::vec2>& hull, float maxDisplacement, std::mt19937& rng)
    {

        std::vector<aiko::vec2> result;
        result.reserve(hull.size() * 2);

        std::uniform_real_distribution<float> angleDistribution(0.0f, aiko::TWO_PI);
        std::uniform_real_distribution<float> magnitudeDistribution(0.0f, maxDisplacement);

        for (size_t i = 0 ; i < hull.size(); ++i)
        {
            const auto& a = hull[i];
            const auto& b = hull[(i + 1) % hull.size()];

            result.push_back(a);

            const auto midpoint = ( a + b ) * 0.5f;

            const auto angle = angleDistribution(rng);
            const auto magnitude = magnitudeDistribution(rng);

            const auto direction = aiko::vec2(aiko::math::cos(angle), aiko::math::sin(angle));
            const auto displaceMidPoint = midpoint + direction * magnitude;

            result.push_back(displaceMidPoint);
        }

        return result;
    }

    bool TrackGenerator::pushPointsApart(std::vector<aiko::vec2>& points, float minDistance)
    {

        if (points.size() < 4)
        {
            return false;
        }

        constexpr size_t nPasses = 3;
        constexpr float tolerance = 1e-4f;

        const auto minDstSquared = minDistance * minDistance;

        for (size_t passes = 0 ; passes < nPasses ; ++passes)
        {

            bool changed = false;

            for (size_t i = 0 ; i < points.size() ; ++i)
            {
                const auto j = (i + 1) % points.size();

                const auto delta = points[j] - points[i];
                const auto dstSquared = aiko::math::lengthSquared(delta);

                if (dstSquared < epsilon * epsilon)
                {
                    return false;
                }

                if (dstSquared >= minDstSquared)
                {
                    continue;
                }

                const auto distance = aiko::math::sqrt(dstSquared);
                const auto direction = delta / distance;
                const auto correction = ( minDistance - distance ) * 0.5f;

                points[i] -= direction * correction;
                points[j] += direction * correction;

                changed = true;

            }

            if (changed == false)
            {
                break;
            }

        }

        const float requiredDistance = std::max(0.0f, minDistance - tolerance);
        for (size_t i = 0 ; i < points.size(); ++i)
        {
            const auto j = (i + 1) % points.size();

            const auto distance = aiko::math::distance(points[j], points[i]);
            if (distance <= epsilon || distance < requiredDistance)
            {
                return false;
            }
        }

        return true;

    }

    bool TrackGenerator::fixAngles(std::vector<aiko::vec2>& points, float maxAngleDegrees)
    {

        if (points.size() < 4)
        {
            return false;
        }

        constexpr float minLengthSquared = epsilon * epsilon;

        const auto maxAngle = aiko::math::radians(maxAngleDegrees);

        for (size_t i = 0 ; i < points.size(); ++i)
        {

            const auto previous = (i + points.size() - 1) % points.size();
            const auto next = (i + 1) % points.size();

            const auto& incoming = points[i] - points[previous];
            const auto& outgoing = points[next] - points[i];

            if (aiko::math::lengthSquared(incoming) <= minLengthSquared || aiko::math::lengthSquared(outgoing) <= minLengthSquared)
            {
                return false;
            }

            const auto cross = incoming.x * outgoing.y - incoming.y * outgoing.x;
            const auto dot = incoming.x * outgoing.x + incoming.y * outgoing.y;

            const auto angle = std::atan2(cross, dot);

            if (std::abs(angle) <= maxAngle)
            {
                continue;
            }

            const auto correctedAngle = aiko::math::clamp(angle, -maxAngle, maxAngle);
            const auto rotation = correctedAngle - angle;

            const float c = aiko::math::cos(rotation);
            const float s = aiko::math::sin(rotation);
            const aiko::vec2 rotatedOutgoing = { outgoing.x * c - outgoing.y * s, outgoing.x * s + outgoing.y * c };

            points[next] = points[i] + rotatedOutgoing;

        }

        return true;

    }

    bool TrackGenerator::validateAngles(const std::vector<aiko::vec2>& points, float maxAngleDegrees)
    {
        if (points.size() < 4)
        {
            return false;
        }

        const float maxAngle = aiko::math::radians(maxAngleDegrees);
        constexpr float tolerance = 1e-4f;

        for (size_t i = 0; i < points.size(); ++i)
        {
            const auto& previous = points[(i + points.size() - 1) % points.size()];
            const auto& current = points[i];
            const auto& next = points[(i + 1) % points.size()];

            const auto incoming = current - previous;
            const auto outgoing = next - current;

            constexpr float epsilonSquared = epsilon * epsilon;

            if (aiko::math::lengthSquared(incoming) <= epsilonSquared || aiko::math::lengthSquared(outgoing) <= epsilonSquared)
            {
                return false;
            }

            const float cross = incoming.x * outgoing.y - incoming.y * outgoing.x;
            const float dot = incoming.x * outgoing.x + incoming.y * outgoing.y;

            const float angle = std::atan2(cross, dot);

            if (aiko::math::isfinite(angle) == false || std::abs(angle) > maxAngle + tolerance)
            {
                return false;
            }
        }

        return true;
    }
}
