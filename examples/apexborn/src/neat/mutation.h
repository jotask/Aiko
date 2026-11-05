#pragma once
#include "genome.h"
#include "innovation_tracker.h"

#include <random>

namespace ab
{

    class Mutation
    {
    public:
        static bool addConnection(Genome& genome, InnovationTracker& tracker, std::mt19937_64& rng);
        static bool addNode(Genome& genome, InnovationTracker& tracker, std::mt19937_64& rng);
        static bool mutateWeights(Genome& genome, std::mt19937_64& rng, double mutationProbability, double replacementProbability, double perturbationSigma);
    };

}
