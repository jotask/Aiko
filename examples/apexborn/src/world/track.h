#pragma once

#include "world/spline.h"

namespace ab
{

    class Track
    {
    public:
        Track();
        ~Track();

        void generate();

        void render(aiko::RenderContext& context);

    private:

        Spline m_centerLine;

        float m_trackWidth = 1.0f;

        Spline::Points m_leftEdge;
        Spline::Points m_rightEdge;

    };

}