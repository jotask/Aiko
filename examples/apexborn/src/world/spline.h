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

        void generate(const Points& controlPoints, aiko::uint subdivisions = 7);

        aiko::vec3 getSampleTangent(size_t splineIdx) const;

        void render(aiko::RenderContext& context);

        const Points& getSamplePoints() const { return m_spline; }

    private:

        aiko::vec3 evaluate(size_t segmentIndex, float u) const;

        Points m_controlPoints;
        Points m_spline;

    };

}