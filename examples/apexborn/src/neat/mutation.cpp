#include "mutation.h"

#include <algorithm>
#include <utility>

namespace ab
{

    bool Mutation::addConnection(Genome& genome, InnovationTracker& tracker, std::mt19937_64& rng)
    {

        if (genome.getNodes().size() < 2)
        {
            return false;
        }

        std::uniform_int_distribution<size_t> nodesDistribution( 0, genome.getNodes().size() - 1 );
        std::uniform_real_distribution<double> weightDistribution( -1.0, 1.0 );

        constexpr size_t MaxAttempts = 32;
        for (size_t attempt = 0 ; attempt < MaxAttempts ; ++attempt)
        {
            const auto sourceIdx = nodesDistribution(rng);
            const auto destinationIdx = nodesDistribution(rng);

            const NodeGene& source = genome.getNodes()[sourceIdx];
            const NodeGene& destination = genome.getNodes()[destinationIdx];

            if (source.type == NodeType::Output)
            {
                continue;
            }

            if (destination.type == NodeType::Input || destination.type == NodeType::Bias)
            {
                continue;
            }

            if (genome.hasConnection(source.id, destination.id) == true || genome.wouldCreateCycle(source.id, destination.id) == true)
            {
                continue;
            }

            const ConnectionGene connection =
            {
                .innovationId = tracker.generateId(source.id, destination.id),
                .fromNode = source.id,
                .toNode = destination.id,
                .weight = weightDistribution(rng),
                .enabled = true,
            };

            return genome.insertConnection(connection);

        }
        return false;
    }

    bool Mutation::addNode(Genome& genome, InnovationTracker& tracker, std::mt19937_64& rng)
    {
        // Select an enabled connection
        std::vector<InnovationId> eligibleConnections;
        const auto& connections = genome.getConnections();
        for (const ConnectionGene& connection : connections)
        {
            if (connection.enabled == false)
            {
                continue;
            }
            eligibleConnections.push_back(connection.innovationId);
        }
        if (eligibleConnections.empty() == true)
        {
            return false;
        }
        std::uniform_int_distribution<size_t> connectionIndexDistribution(0, eligibleConnections.size() - 1);
        const InnovationId randomEligibleConnection = eligibleConnections[connectionIndexDistribution(rng)];
        auto found = std::find_if(connections.begin(), connections.end(), [randomEligibleConnection](const ConnectionGene& connection){ return connection.innovationId == randomEligibleConnection; });
        if (found == connections.end())
        {
            return false;
        }
        // Obtain the historical split
        const ConnectionGene selectedConnection = *found;
        const auto splitInnovation = tracker.generateSplitInnovation(selectedConnection.innovationId, selectedConnection.fromNode, selectedConnection.toNode);
        if (splitInnovation == std::nullopt)
        {
            return false;
        }
        // Create the replacement genes
        const NodeGene hiddenGene = {
            .id = splitInnovation->hiddenNode,
            .type = NodeType::Hidden,
        };
        const ConnectionGene incomingConnection = {
            .innovationId = splitInnovation->inInnovationId,
            .fromNode = selectedConnection.fromNode,
            .toNode = hiddenGene.id,
            .weight = 1.0,
            .enabled = true,
        };
        const ConnectionGene outgoingConnection = {
            .innovationId = splitInnovation->outInnovationId,
            .fromNode = hiddenGene.id,
            .toNode = selectedConnection.toNode,
            .weight = selectedConnection.weight,
            .enabled = true,
        };
        // Apply the mutation atomically
        Genome candidateGenome = genome;
        if (candidateGenome.setConnectionEnabled(selectedConnection.innovationId, false) == false)
        {
            return false;
        }
        if (candidateGenome.insertNode(hiddenGene) == false)
        {
            return false;
        }
        if (candidateGenome.insertConnection(incomingConnection) == false)
        {
            return false;
        }
        if (candidateGenome.insertConnection(outgoingConnection) == false)
        {
            return false;
        }
        genome = std::move(candidateGenome);
        return true;
    }
}
