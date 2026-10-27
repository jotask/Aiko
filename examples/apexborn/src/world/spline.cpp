#include "spline.h"

#include "core/utils.h"

namespace ab
{

    Spline::Spline()
    {
        m_controlPoints.push_back({-2.0f, -2.0f, 0.0f});
        m_controlPoints.push_back({-2.0f,  2.0f, 0.0f});
        m_controlPoints.push_back({ 2.0f,  2.0f, 0.0f});
        m_controlPoints.push_back({ 2.0f, -2.0f, 0.0f});
    }

    Spline::~Spline() = default;

    void Spline::generate(aiko::uint subdivisions)
    {
        m_spline.clear();
        m_spline.reserve(m_controlPoints.size() * subdivisions);

        AIKO_ASSERT(m_controlPoints.size() >= 4, "Spline should contains at least four points");
        AIKO_ASSERT(subdivisions > 0, "We can't generate 0 subdivision");

        constexpr uint temporalSamples = 100;

        std::vector<aiko::vec3> denseSamples;
        denseSamples.reserve(temporalSamples * m_controlPoints.size());

        for (size_t i = 0; i < m_controlPoints.size() ; ++i)
        {
            for (size_t subsegment = 0 ; subsegment < temporalSamples; ++subsegment)
            {
                const auto u = aiko::math::map(static_cast<float>(subsegment), static_cast<float>(0), static_cast<float>(temporalSamples), 0.0f, 1.0f);
                const auto point = evaluate(i, u);
                denseSamples.push_back(point);
            }
        }

        std::vector<float> cumulativeDistances;
        cumulativeDistances.reserve(temporalSamples * m_controlPoints.size());

        cumulativeDistances.push_back(0.0f);
        float totalLength = 0;
        for (size_t sample = 1 ; sample < denseSamples.size(); ++sample)
        {
            const float distance = aiko::math::length(denseSamples[sample - 0] - denseSamples[sample - 1]);
            totalLength += distance;
            cumulativeDistances.push_back(totalLength);
        }
        const float distance = aiko::math::length(denseSamples[denseSamples.size() - 1] - denseSamples[0]);
        totalLength += distance;
        denseSamples.push_back(denseSamples[0]);
        cumulativeDistances.push_back(totalLength);

        const size_t sampleCount = m_controlPoints.size() * subdivisions;
        const float spacing = totalLength / sampleCount;

        for (size_t sampleIdx = 0; sampleIdx < sampleCount ; ++sampleIdx)
        {
            const float targetDistance = sampleIdx * spacing;
            for (size_t cumulativeIdx = 1; cumulativeIdx < cumulativeDistances.size() ; ++cumulativeIdx)
            {
                if (cumulativeDistances[cumulativeIdx] < targetDistance)
                {
                    continue;
                }
                const auto rightCumulative = cumulativeDistances[cumulativeIdx - 0];
                const auto leftCumulative = cumulativeDistances[cumulativeIdx - 1];

                const float interpolationFactor = (targetDistance - leftCumulative) / (rightCumulative - leftCumulative);

                const auto rightSample = denseSamples[cumulativeIdx - 0];
                const auto leftSample = denseSamples[cumulativeIdx - 1];

                const auto point = aiko::math::lerp(leftSample, rightSample, interpolationFactor);

                m_spline.push_back(point);

                break;
            }
        }
    }

    aiko::vec3 Spline::evaluate(size_t i, float u) const
    {

        const auto N = m_controlPoints.size();

        // Control points
        const aiko::vec3& p0 = m_controlPoints[( i + N - 1) % N];
        const aiko::vec3& p1 = m_controlPoints[i];
        const aiko::vec3& p2 = m_controlPoints[(i + 1) % N];
        const aiko::vec3& p3 = m_controlPoints[(i + 2) % N];

        const float t0 = 0.0f;
        const float t1 = t0 + aiko::math::sqrt(aiko::math::length(p1 - p0));
        const float t2 = t1 + aiko::math::sqrt(aiko::math::length(p2 - p1));
        const float t3 = t2 + aiko::math::sqrt(aiko::math::length(p3 - p2));

        // Interpolation
        const float t = t1 + u * ( t2 - t1);
        const float t01 = (t - t0) / ( t1 - t0);
        const float t12 = (t - t1) / ( t2 - t1);
        const float t23 = (t - t2) / ( t3 - t2);

        const auto a1 = aiko::math::lerp(p0, p1, t01);
        const auto a2 = aiko::math::lerp(p1, p2, t12);
        const auto a3 = aiko::math::lerp(p2, p3, t23);

        const auto tb1 = (t - t0) / (t2 - t0);
        const auto tb2 = (t - t1) / (t3 - t1);

        const auto b1 = aiko::math::lerp(a1, a2, tb1);
        const auto b2 = aiko::math::lerp(a2, a3, tb2);

        const auto tc = (t - t1) / (t2 - t1);

        const auto c3 = aiko::math::lerp(b1, b2, tc);

        return c3;
    }

    aiko::vec3 Spline::getSampleTangent(size_t splineIdx) const
    {
        AIKO_ASSERT(m_spline.size() >= 3, "We need 3 points to calculate the tangent");
        AIKO_ASSERT(splineIdx < m_spline.size(), "Spline index OOB for tangent");

        const auto N = m_spline.size();

        const auto previousSample = m_spline[(splineIdx+ N - 1) % N];
        const auto nextSample = m_spline[(splineIdx + 1) % N];

        const auto direction = nextSample - previousSample;
        const auto normalizedDirection = aiko::math::normalize(direction);
        return normalizedDirection;
    }

    void Spline::render(aiko::RenderContext& context)
    {
        for (const auto& controlPoint : m_controlPoints)
        {
            context.drawCircle(controlPoint, {0.25f}, 16, aiko::MAGENTA);
        }

        if (m_spline.empty())
        {
            return;
        }

        for (size_t i = 1; i < m_spline.size() ; ++i)
        {
            const auto& begin = m_spline[i-1];
            const auto& end = m_spline[i];
            context.drawLine(begin, end, aiko::RED);
            const auto tangent = getSampleTangent(i);

            constexpr float arrowLength = 0.5f;

            const auto tangentStart = end;
            const auto tangentEnd = tangentStart + tangent * arrowLength;
            context.drawLine(tangentStart, tangentEnd, aiko::BLUE);
        }

        for (const auto& spline : m_spline)
        {
            context.drawPoint(spline, aiko::GREEN);
        }

        const auto& begin = m_spline[m_spline.size() - 1];
        const auto& end = m_spline[0];
        context.drawLine(begin, end, aiko::RED);
    }

}
