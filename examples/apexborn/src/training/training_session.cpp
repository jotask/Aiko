#include "training_session.h"

#include "driving_genome_factory.h"
#include "simulation/network_action_adapter.h"

namespace ab
{

    bool TrainingSession::init(const EvolutionConfig& config, std::uint64_t seed)
    {

        const auto genome = DrivingGenomeFactory::createSeedGenome(11, 2);
        if (genome == std::nullopt)
        {
            return false;
        }

        Population population;
        bool populationSuccess = population.initialize(genome.value(), config, seed);
        if (populationSuccess == false)
        {
            return false;
        }

        std::vector<NeuralNetwork> networks;
        for (const auto& genome : population.genomes())
        {
            networks.emplace_back();
            if (networks.back().build(genome) == false)
            {
                return false;
            }
        }

        if (population.genomes().size() != networks.size())
        {
            return false;
        }

        m_population = std::move(population);
        m_networks = std::move(networks);

        return true;
    }

    bool TrainingSession::evaluate(size_t agentIdx, const DrivingObservationData& observations, VehicleControl& controls)
    {
        if (agentIdx >= populationSize() || agentIdx >= m_networks.size())
        {
            return false;
        }

        const std::array<double, DrivingObservationData::TotalMembers> inputs = DrivingObservation::generateInputs(observations);

        std::array<double, 2> outputs;

        bool success = m_networks[agentIdx].evaluate(inputs, outputs);
        if (success == false)
        {
            return false;
        }

        for (const double out : outputs)
        {
            if (aiko::math::isfinite(out) == false)
            {
                return false;
            }
        }

        const VehicleControl control = NetworkActionAdapter::toControls(outputs[0], outputs[1]);

        controls = control;

        return true;

    }

    bool TrainingSession::finishGeneration(const std::vector<VehicleAgent>& agents)
    {
        if (populationSize() == 0)
        {
            return false;
        }
        if (populationSize() != agents.size())
        {
            return false;
        }
        if (populationSize() != m_networks.size())
        {
            return false;
        }

        for (const VehicleAgent& agent : agents)
        {
            if (agent.alive == true)
            {
                return false;
            }
            if (aiko::math::isfinite(agent.bestProgress) == false || agent.bestProgress < 0.0f)
            {
                return false;
            }
        }

        Population population = m_population;
        for (size_t i = 0 ; i < agents.size() ; ++i)
        {
            if (population.setFitness(i, agents[i].bestProgress) == false)
            {
                return false;
            }
        }
        if (population.advanceGeneration() == false)
        {
            return false;
        }

        std::vector<NeuralNetwork> networks;
        for (size_t i = 0 ; i < population.genomes().size() ; ++i)
        {
            NeuralNetwork network;
            if (network.build(population.genomes()[i]) == false)
            {
                return false;
            }
            networks.push_back(network);
        }

        if (networks.size() != population.genomes().size())
        {
            return false;
        }

        m_population = std::move(population);
        m_networks = std::move(networks);

        return true;

    }

}
