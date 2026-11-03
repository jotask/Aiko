#pragma once

#include "spline.h"

namespace ab
{

    class TrackNavigation final
    {
    public:

        struct TrackProjection
        {
            aiko::vec3 closestToCenterLine;
            size_t segmentIndex;
            float interpolation;
            float distanceFromCenter;
            float progressOnTrack;
        };

        void recalculate(const Spline& spline);

        TrackProjection getTrackProjectionFromPoint(const aiko::vec3& point) const;

    private:

        TrackProjection calculateTrackProjection(const size_t startSegment, const size_t endSegment, const aiko::vec3& point) const;

        Spline::Points m_centerLine; // Copy of the spline itself

        std::vector<float> m_segmentCumulativeLength;
        float m_trackTotalLength = 0.0f;

    };

}
