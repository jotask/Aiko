#include "track_navigation.h"

#include "constant_types.h"

#include <intrumentor/profiler.h>

#include <algorithm>
#include <cmath>
#include <limits>

namespace ab
{

    void TrackNavigation::recalculate(const Spline& spline)
    {

        AIKO_ASSERT(spline.getSamplePoints().size() >= 3 , "No enough points for track navigation");

        m_centerLine.clear();
        m_segmentCumulativeLength.clear();

        m_centerLine = spline.getSamplePoints();

        m_segmentCumulativeLength.push_back(0.0f);
        m_trackTotalLength = 0.0f;

        // Cumulative distance
        for (size_t i = 0 ; i < m_centerLine.size(); ++i)
        {
            const aiko::vec3& current = m_centerLine[i];
            const aiko::vec3& next = m_centerLine[(i + 1) % m_centerLine.size()];
            const float distance = aiko::math::distance(current, next);
            AIKO_ASSERT(distance > epsilon, "Distance between two points too close?");
            m_segmentCumulativeLength.push_back(m_segmentCumulativeLength.back() + distance);
            m_trackTotalLength += distance;
        }
        AIKO_ASSERT(m_trackTotalLength > epsilon, "Total track length too short?");
    }


    TrackNavigation::TrackProjection TrackNavigation::getTrackProjectionFromPoint(const aiko::vec3& point) const
    {
        AIKO_FUNCTION_PROFILE

        AIKO_ASSERT(m_centerLine.size() >= 3, "Track navigation has not been initialized");

        const size_t segmentCount = m_centerLine.size();

        float bestDistanceSquared = std::numeric_limits<float>::max();
        size_t bestSegmentIndex = 0;

        for (size_t i = 0; i < segmentCount; ++i)
        {
            const auto& start = m_centerLine[i];
            const auto& end = m_centerLine[(i + 1) % segmentCount];

            const auto direction = end - start;
            const auto toPoint = point - start;

            const float lengthSquared = aiko::math::dot(direction, direction);

            AIKO_ASSERT(lengthSquared > epsilon * epsilon, "Invalid navigation segment");

            const float t = aiko::math::clamp(aiko::math::dot(toPoint, direction) / lengthSquared, 0.0f, 1.0f);

            const auto closestPoint = start + direction * t;
            const auto difference = point - closestPoint;

            const float distanceSquared = aiko::math::dot(difference, difference);

            if (distanceSquared < bestDistanceSquared)
            {
                bestDistanceSquared = distanceSquared;
                bestSegmentIndex = i;
            }
        }

        return calculateTrackProjection(bestSegmentIndex, (bestSegmentIndex + 1) % segmentCount, point);
    }

    aiko::vec3 TrackNavigation::pointAtDistance(float distance) const
    {
        AIKO_ASSERT(m_centerLine.size() >= 3, "Track navigation is not initialized");
        AIKO_ASSERT(std::isfinite(distance), "Invalid track distance");

        float wrapped = std::fmod(distance, m_trackTotalLength);

        if (wrapped < 0.0f)
        {
            wrapped += m_trackTotalLength;
        }

        const auto it = std::upper_bound( m_segmentCumulativeLength.begin(), m_segmentCumulativeLength.end(), wrapped);

        const size_t index = static_cast<size_t>(std::distance(m_segmentCumulativeLength.begin(), it)) - 1;

        const float start = m_segmentCumulativeLength[index];
        const float end = m_segmentCumulativeLength[index + 1];

        const float t = (wrapped - start) / (end - start);

        const auto& p0 = m_centerLine[index];
        const auto& p1 = m_centerLine[(index + 1) % m_centerLine.size()];

        return p0 + (p1 - p0) * t;
    }

    TrackNavigation::TrackProjection TrackNavigation::calculateTrackProjection(const size_t startIdx, const size_t endIdx, const aiko::vec3& point) const
    {
        const auto& startPoint = m_centerLine[startIdx];
        const auto& endPoint = m_centerLine[endIdx];
        const aiko::vec3 segmentDirection = endPoint - startPoint;
        const aiko::vec3 directionTowardsPoint = point - startPoint;
        const auto lengthSquared = aiko::math::dot(segmentDirection, segmentDirection);
        AIKO_ASSERT(lengthSquared > epsilon * epsilon, "Invalid navigation segment");
        const aiko::vec3 normalizeDirection = aiko::math::normalize(segmentDirection);
        const float dot = aiko::math::dot(directionTowardsPoint, segmentDirection);
        const float t = aiko::math::clamp(dot / lengthSquared, 0.0f, 1.0f);
        const aiko::vec3 pointOnTrack = startPoint + t * segmentDirection;
        const float distanceFromCentre = aiko::math::distance(point, pointOnTrack);
        const auto segmentLength = m_segmentCumulativeLength[startIdx + 1] - m_segmentCumulativeLength[startIdx];
        const float progressOnTrack = m_segmentCumulativeLength[startIdx] + t  * segmentLength;
        const auto right = aiko::math::normalize(aiko::math::cross(segmentDirection, WorldUp));
        const auto v = point - pointOnTrack;
        const auto signedLateralOffset = aiko::math::dot(v, right);

        return {
            .forwardDirection = normalizeDirection,
            .closestToCenterLine = pointOnTrack,
            .segmentIndex = startIdx,
            .interpolation = t,
            .distanceFromCenter = distanceFromCentre,
            .progressOnTrack = progressOnTrack,
            .signedLateralOffset = signedLateralOffset,
        };
    }

}
