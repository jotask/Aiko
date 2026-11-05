#pragma once

#include "evolution_config.h"
#include "genome.h"
#include "innovation_tracker.h"
#include "species.h"

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

namespace ab
{
    class Population
    {
    public:

        Population();

        bool initialize(const Genome& seedGenome, const EvolutionConfig& config, std::uint64_t randomSeed);
        bool advanceGeneration();

        bool setFitness(std::size_t genomeIndex, double fitness);

        const auto& genomes() const { return m_genomes; }
        const auto& species() const { return m_species; }
        const uint64_t generation() const { return m_generation; }

    private:
        std::vector<Genome> m_genomes;
        std::vector<Species> m_species;
        std::vector<double> m_fitness;
        std::vector<bool> m_fitnessEvaluated;
        EvolutionConfig m_config;
        InnovationTracker m_innovationTracker;
        std::mt19937_64 m_rng;
        std::uint64_t m_generation = 0;
        std::uint64_t m_nextSpeciesId = 1;
    };
}
