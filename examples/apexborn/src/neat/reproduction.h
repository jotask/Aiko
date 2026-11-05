#pragma once

#include <vector>
#include <span>
#include <random>

namespace ab
{

    class Genome;
    class InnovationTracker;
    struct Species;
    struct EvolutionConfig;

    class Reproduction final
    {
    public:
        static bool reproduce(const std::vector<Genome>& genomes, std::span<const double> fitness, const std::vector<Species>& species, const EvolutionConfig& config, InnovationTracker& innovationTracker, std::mt19937_64& rng, std::vector<Genome>& offspring);
    };

}