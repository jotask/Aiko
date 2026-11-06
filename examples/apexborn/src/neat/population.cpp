#include "population.h"

#include "math/math.h"
#include "mutation.h"
#include "neural_network.h"
#include "reproduction.h"
#include "speciation.h"

#include <algorithm>
#include <limits>
#include <random>
#include <unordered_set>
#include <utility>

namespace ab
{

    Population::Population()
        : m_innovationTracker(InvalidNodeId + 1)
    {

    }

    bool Population::initialize(const Genome& seedGenome, const EvolutionConfig& config, std::uint64_t randomSeed)
    {

        if (config.validate() == false)
        {
            return false;
        }

        // Validate seed genome nodes.
        const auto& nodes = seedGenome.getNodes();
        const auto& connections = seedGenome.getConnections();

        if (nodes.empty())
        {
            return false;
        }

        std::unordered_set<NodeId> nodeIds;
        NodeId maxNodeId = InvalidNodeId;

        bool hasInput = false;
        bool hasOutput = false;

        for (const NodeGene& node : nodes)
        {
            if (node.id == InvalidNodeId)
            {
                return false;
            }

            if (nodeIds.insert(node.id).second == false)
            {
                return false;
            }

            maxNodeId = std::max(maxNodeId, node.id);

            if (node.type == NodeType::Input)
            {
                hasInput = true;
            }
            else if (node.type == NodeType::Output)
            {
                hasOutput = true;
            }
        }

        if (hasInput == false || hasOutput == false)
        {
            return false;
        }

        if (maxNodeId == std::numeric_limits<NodeId>::max())
        {
            return false;
        }

        // Validate seed genome connections.
        std::unordered_set<InnovationId> innovationIds;
        InnovationId maxInnovationId = InvalidInnovationId;

        for (const ConnectionGene& connection : connections)
        {
            if (connection.innovationId == InvalidInnovationId || connection.fromNode == InvalidNodeId || connection.toNode == InvalidNodeId)
            {
                return false;
            }

            if (nodeIds.contains(connection.fromNode) == false || nodeIds.contains(connection.toNode) == false)
            {
                return false;
            }

            if (aiko::math::isfinite(connection.weight) == false)
            {
                return false;
            }

            if (innovationIds.insert(connection.innovationId).second == false)
            {
                return false;
            }

            maxInnovationId = std::max(maxInnovationId, connection.innovationId);
        }

        if (maxInnovationId == std::numeric_limits<InnovationId>::max())
        {
            return false;
        }

        // Verify that the seed can build a neural network.
        NeuralNetwork network;
        if (network.build(seedGenome) == false)
        {
            return false;
        }

        const NodeId nextNodeId = maxNodeId + 1;
        const InnovationId nextInnovationId = maxInnovationId + 1;

        InnovationTracker localTracker(nextNodeId, nextInnovationId);
        std::mt19937_64 localRng(randomSeed);

        std::vector<Genome> localGenomes;
        localGenomes.reserve(config.populationSize);

        // Create the initial population.
        for (std::size_t i = 0; i < config.populationSize; ++i)
        {
            Genome genome = seedGenome;
            Mutation::mutateWeights(genome, localRng, 1.0, 1.0, 0.0 );
            localGenomes.push_back(std::move(genome));
        }

        // Initialize fitness tracking.
        std::vector<double> localFitness(config.populationSize, 0.0);
        std::vector<bool> localEvaluated(config.populationSize, false);

        // Assign initial species.
        std::vector<Species> localSpecies;
        std::uint64_t localNextSpeciesId = 1;

        if (Speciation::assign(localGenomes, config.compatibilityThreshold, config.excessCoefficient, config.disjointCoefficient, config.weightCoefficient, localSpecies, localNextSpeciesId) == false)
        {
            return false;
        }

        // Commit the initialized population.
        m_genomes = std::move(localGenomes);
        m_species = std::move(localSpecies);
        m_fitness = std::move(localFitness);
        m_fitnessEvaluated = std::move(localEvaluated);

        m_config = config;
        m_innovationTracker = std::move(localTracker);
        m_rng = std::move(localRng);

        m_generation = 0;
        m_nextSpeciesId = localNextSpeciesId;

        return true;

    }

    bool Population::advanceGeneration()
    {
        // Validate current population state.
        if (m_genomes.empty() ||
            m_genomes.size() != m_config.populationSize ||
            m_fitness.size() != m_genomes.size() ||
            m_fitnessEvaluated.size() != m_genomes.size() ||
            m_generation == std::numeric_limits<std::uint64_t>::max())
        {
            return false;
        }

        // Every genome must have an evaluated fitness.
        if (std::all_of(
                m_fitnessEvaluated.begin(),
                m_fitnessEvaluated.end(),
                [](bool evaluated) { return evaluated; }) == false)
        {
            return false;
        }

        // Prepare local state for atomic generation advancement.
        InnovationTracker localTracker = m_innovationTracker;
        std::mt19937_64 localRng = m_rng;
        std::vector<Species> localSpecies = m_species;
        std::uint64_t localNextSpeciesId = m_nextSpeciesId;

        // Produce the next generation.
        std::vector<Genome> offspring;

        if (Reproduction::reproduce(m_genomes, m_fitness, m_species, m_config, localTracker, localRng, offspring) == false)
        {
            return false;
        }

        if (offspring.size() != m_config.populationSize)
        {
            return false;
        }

        if (Speciation::assign(offspring, m_config.compatibilityThreshold, m_config.excessCoefficient, m_config.disjointCoefficient, m_config.weightCoefficient, localSpecies, localNextSpeciesId) == false)
        {
            return false;
        }

        std::vector<double> localFitness(offspring.size(), 0.0);
        std::vector<bool> localEvaluated(offspring.size(), false);

        localTracker.clearGenerationHistory();

        m_genomes = std::move(offspring);
        m_species = std::move(localSpecies);
        m_fitness = std::move(localFitness);
        m_fitnessEvaluated = std::move(localEvaluated);

        m_innovationTracker = std::move(localTracker);
        m_rng = std::move(localRng);
        m_nextSpeciesId = localNextSpeciesId;

        ++m_generation;

        return true;
    }

    bool Population::setFitness(std::size_t genomeIndex, double fitness)
    {
        if (genomeIndex >= m_genomes.size() || genomeIndex >= m_fitness.size() || genomeIndex >= m_fitnessEvaluated.size())
        {
            return false;
        }

        if (aiko::math::isfinite(fitness) == false)
        {
            return false;
        }

        m_fitness[genomeIndex] = fitness;
        m_fitnessEvaluated[genomeIndex] = true;

        return true;
    }
}
