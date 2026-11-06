#pragma once
#include "neat/neural_network.h"
#include "neat/population.h"
#include "simulation/driving_observation.h"
#include "simulation/vehicle_agent.h"
#include "simulation/vehicle_controls.h"

namespace ab
{

    class TrainingSession
    {
    public:

        bool init(const EvolutionConfig& config, std::uint64_t seed);
        bool evaluate(size_t agentIdx, const DrivingObservationData& observations, VehicleControl& controls, std::vector<NodeActivation>* activations = nullptr);
        bool finishGeneration(const std::vector<VehicleAgent>& agents, const float trackLength, float maxEpisodeDuration);

        const Genome* getBestGenome() const { return m_bestGenome == std::nullopt ? nullptr : &m_bestGenome.value(); }
        const double getBestFitness() const { return m_bestFitness; }
        const uint64_t getBestGeneration() const { return m_bestGeneration; }

        const Genome* getGenome(size_t agentIdx) const;

        size_t populationSize() const { return m_population.genomes().size(); }
        uint64_t generation() const { return m_population.generation(); }

    private:
        Population m_population;
        std::vector<NeuralNetwork> m_networks;

        std::optional<Genome> m_bestGenome = std::nullopt;
        double m_bestFitness = 0.0f;
        uint64_t m_bestGeneration = 0;
    };
}
