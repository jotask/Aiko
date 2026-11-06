#include "driving_genome_factory.h"

#include "neat/genome.h"

#include <algorithm>
#include <ranges>

namespace ab
{

    std::optional<Genome> DrivingGenomeFactory::createSeedGenome(size_t input, size_t output)
    {

        if (input == 0 || output == 0)
        {
            return std::nullopt;
        }

        Genome genome;

        NodeId genomeIdCounter = InvalidNodeId;
        for (size_t i = 0 ; i < input ; ++i)
        {
            if (genome.insertNode({++genomeIdCounter, NodeType::Input}) == false)
            {
                return std::nullopt;
            }
        }

        if (genome.insertNode({++genomeIdCounter, NodeType::Bias}) == false)
        {
            return std::nullopt;
        }

        for (size_t i = 0 ; i < output ; ++i)
        {
            if (genome.insertNode({++genomeIdCounter, NodeType::Output}) == false)
            {
                return std::nullopt;
            }
        }

        const std::vector<NodeGene>& nodes = genome.getNodes();
        auto inputGenomesView = nodes | std::ranges::views::filter([](const NodeGene& gene)-> bool { return gene.type == NodeType::Input || gene.type == NodeType::Bias; });
        auto outputGenomesView = nodes | std::ranges::views::filter([](const NodeGene& gene)-> bool { return gene.type == NodeType::Output; });

        const std::vector<NodeGene> inputGenomes(inputGenomesView.begin(), inputGenomesView.end());
        const std::vector<NodeGene> outputGenomes(outputGenomesView.begin(), outputGenomesView.end());

        InnovationId innovationIdCounter = InvalidInnovationId;
        for (size_t i = 0 ; i < inputGenomes.size() ; ++i)
        {
            const NodeGene& from = inputGenomes[i];
            for (size_t j = 0 ; j < outputGenomes.size() ; ++j)
            {
                const NodeGene& to = outputGenomes[j];
                const ConnectionGene connection = {
                    .innovationId = ++innovationIdCounter,
                    .fromNode = from.id,
                    .toNode = to.id,
                    .weight = 0.0,
                    .enabled = true,
                };
                if (genome.insertConnection(connection) == false)
                {
                    return std::nullopt;
                }
            }
        }

        return genome;
    }

}
