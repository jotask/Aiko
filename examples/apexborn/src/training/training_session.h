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
        bool evaluate(size_t agentIdx, const DrivingObservationData& observations, VehicleControl& controls);
        bool finishGeneration(const std::vector<VehicleAgent>& agents);

        size_t populationSize() const { return m_population.genomes().size(); }
        uint64_t generation() const { return m_population.generation(); }

    private:
        Population m_population;
        std::vector<NeuralNetwork> m_networks;
    };
}
