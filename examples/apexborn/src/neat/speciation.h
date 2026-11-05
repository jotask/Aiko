#pragma once

#include <cstdint>
#include <vector>

namespace ab
{
    struct Species;
    class Genome;
    class Speciation final
    {
    public:
        static bool assign(const std::vector<Genome>& genomes, double compatibilityThreshold, double excessCoefficient, double disjointCoefficient, double weightCoefficient, std::vector<Species>& species, std::uint64_t& nextSpeciesId);
    };
}
