#pragma once

#include "layers/contexts/render_context.h"

#include <math/math.h>

#include <vector>

namespace ab
{

    class Spline
    {
    public:

        using Points = std::vector<aiko::vec3>;

        Spline();
        ~Spline();

        void generate(aiko::uint subdivisions = 7);

        void render(aiko::RenderContext& context);

    private:

        aiko::vec3 evaluate(size_t segmentIndex, float u) const;
        aiko::vec3 getSampleTangent(size_t splineIdx) const;

        Points m_controlPoints;
        Points m_spline;

    };

}