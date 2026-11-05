#pragma once
#include <random>

namespace ab
{
    class Genome;
    class Crossover
    {
    public:
        static bool crossover(const Genome& genomeA, const Genome& genomeB, double fitnessA, double fitnessB, std::mt19937_64& rng, Genome& offspring);
    };
}
