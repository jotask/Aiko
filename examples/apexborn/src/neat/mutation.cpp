#include "mutation.h"

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
}
