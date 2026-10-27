#include "track.h"

namespace ab
{

    namespace
    {
        constexpr aiko::vec3 WorldUp = {0.0f, 0.0f, 1.0f}; // FIXME : Make up the Y axis
    }

    Track::Track() = default;

    Track::~Track() = default;

    void Track::generate()
    {
        m_centerLine.generate();

        m_leftEdge.clear();
        m_rightEdge.clear();

        const auto& samples = m_centerLine.getSamplePoints();

        m_leftEdge.reserve(samples.size());
        m_rightEdge.reserve(samples.size());

        AIKO_ASSERT(m_trackWidth > 0.0f, "Track width must be positive");
        const float halfWidth = m_trackWidth * 0.5f;

        for (size_t idx = 0 ; idx < samples.size(); ++idx)
        {
            const auto tangent = m_centerLine.getSampleTangent(idx);
            const auto rightDirection = aiko::math::normalize(aiko::math::cross(tangent, WorldUp));

            const auto position = samples[idx];

            const auto leftPoint = position - rightDirection * halfWidth;
            const auto rightPoint = position + rightDirection * halfWidth;

            m_leftEdge.emplace_back(leftPoint);
            m_rightEdge.emplace_back(rightPoint);

        }

    }

    void Track::render(aiko::RenderContext& context)
    {
        m_centerLine.render(context);

        if (m_leftEdge.empty() || m_rightEdge.empty())
        {
            return;
        }

        for (size_t i = 1; i < m_leftEdge.size() ; ++i)
        {
            const auto& begin = m_leftEdge[i-1];
            const auto& end = m_leftEdge[i];
            context.drawLine(begin, end, aiko::BROWN);
        }
        context.drawLine(m_leftEdge.back(), m_leftEdge.front(), aiko::BROWN);

        for (size_t i = 1; i < m_rightEdge.size() ; ++i)
        {
            const auto& begin = m_rightEdge[i-1];
            const auto& end = m_rightEdge[i];
            context.drawLine(begin, end, aiko::CYAN);
        }
        context.drawLine(m_rightEdge.back(), m_rightEdge.front(), aiko::CYAN);

    }

}
