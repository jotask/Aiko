#pragma once

#include <optional>

namespace ab
{
    class Genome;

    class CompatibilityDistance
    {
    public:
        static std::optional<double> calculate(const Genome& genomeA, const Genome& genomeB, double excessCoefficient, double disjointCoefficient, double weightCoefficient);
    };

}
