#include "track_navigation.h"

#include "constant_types.h"

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
        AIKO_ASSERT(m_centerLine.size() >= 3, "Track navigation has not been initialized");
        TrackProjection bestProjection = {};
        float bestDistance = std::numeric_limits<float>::max();
        for (size_t i = 0; i < m_centerLine.size() ; ++i)
        {
            const auto startIdx = i;
            const auto endIdx = (i + 1) % m_centerLine.size();
            const auto projection = calculateTrackProjection(startIdx, endIdx, point);
            if (projection.distanceFromCenter < bestDistance)
            {
                bestDistance = projection.distanceFromCenter;
                bestProjection = projection;
            }
        }
        return bestProjection;
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
