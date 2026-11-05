
#include "crossover.h"

#include "genome.h"
#include "math/math.h"

#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace ab
{

    bool Crossover::crossover(const Genome& genomeA, const Genome& genomeB, double fitnessA, double fitnessB, std::mt19937_64& rng, Genome& offspring)
    {

        if (aiko::math::isfinite(fitnessA) == false || aiko::math::isfinite(fitnessB) == false)
        {
            return false;
        }

        // nullptr represents equal fitness.
        const Genome* fitterParent = nullptr;

        if (fitnessA > fitnessB)
        {
            fitterParent = &genomeA;
        }
        else if (fitnessB > fitnessA)
        {
            fitterParent = &genomeB;
        }

        Genome candidateOffspring;

        const auto& connectionsA = genomeA.getConnections();
        const auto& connectionsB = genomeB.getConnections();

        std::size_t indexA = 0;
        std::size_t indexB = 0;

        std::vector<ConnectionGene> inheritedConnections;
        inheritedConnections.reserve(connectionsA.size() + connectionsB.size());

        std::bernoulli_distribution selectionDistribution(0.50);
        std::bernoulli_distribution disableDistribution(0.75);

        while (indexA < connectionsA.size() && indexB < connectionsB.size())
        {
            const ConnectionGene& connectionA = connectionsA[indexA];
            const ConnectionGene& connectionB = connectionsB[indexB];

            if (connectionA.innovationId == connectionB.innovationId)
            {
                if (connectionA.fromNode != connectionB.fromNode || connectionA.toNode != connectionB.toNode)
                {
                    return false;
                }

                ConnectionGene inherited = selectionDistribution(rng) ? connectionA : connectionB;
                if (connectionA.enabled == false || connectionB.enabled == false)
                {
                    inherited.enabled = !disableDistribution(rng);
                }

                inheritedConnections.push_back(inherited);

                ++indexA;
                ++indexB;
            }
            else if (connectionA.innovationId < connectionB.innovationId)
            {
                if (fitterParent == &genomeA || (fitterParent == nullptr && selectionDistribution(rng)))
                {
                    inheritedConnections.push_back(connectionA);
                }

                ++indexA;
            }
            else
            {
                if (fitterParent == &genomeB || (fitterParent == nullptr && selectionDistribution(rng)))
                {
                    inheritedConnections.push_back(connectionB);
                }

                ++indexB;
            }
        }

        while (indexA < connectionsA.size())
        {
            const ConnectionGene& connection = connectionsA[indexA];
            if (fitterParent == &genomeA || (fitterParent == nullptr && selectionDistribution(rng)))
            {
                inheritedConnections.push_back(connection);
            }

            ++indexA;
        }

        while (indexB < connectionsB.size())
        {
            const ConnectionGene& connection = connectionsB[indexB];
            if (fitterParent == &genomeB || (fitterParent == nullptr && selectionDistribution(rng)))
            {
                inheritedConnections.push_back(connection);
            }

            ++indexB;
        }

        // Validate node definitions.
        const auto& nodesA = genomeA.getNodes();
        const auto& nodesB = genomeB.getNodes();

        std::unordered_map<NodeId, NodeGene> nodeLookup;
        std::unordered_map<NodeId, NodeType> mandatoryA;
        std::unordered_map<NodeId, NodeType> mandatoryB;

        for (const NodeGene& node : nodesA)
        {
            if (nodeLookup.emplace(node.id, node).second == false)
            {
                return false;
            }

            if (node.type != NodeType::Hidden)
            {
                mandatoryA.emplace(node.id, node.type);
            }
        }

        for (const NodeGene& node : nodesB)
        {
            const auto [it, inserted] = nodeLookup.emplace(node.id, node);
            if (inserted == false && it->second.type != node.type)
            {
                return false;
            }

            if (node.type != NodeType::Hidden)
            {
                if (mandatoryB.emplace(node.id, node.type).second == false)
                {
                    return false;
                }
            }
        }

        if (mandatoryA.size() != mandatoryB.size())
        {
            return false;
        }

        for (const auto& [nodeId, nodeType] : mandatoryA)
        {
            const auto it = mandatoryB.find(nodeId);
            if (it == mandatoryB.end() || it->second != nodeType)
            {
                return false;
            }
        }

        // Determine which nodes the offspring requires.
        std::unordered_set<NodeId> requiredNodes;
        for (const auto& [nodeId, nodeType] : mandatoryA)
        {
            requiredNodes.insert(nodeId);
        }

        for (const ConnectionGene& connection : inheritedConnections)
        {
            requiredNodes.insert(connection.fromNode);
            requiredNodes.insert(connection.toNode);
        }

        // Insert required nodes into the offspring.
        std::unordered_set<NodeId> insertedNodes;
        for (const NodeGene& node : nodesA)
        {
            if (requiredNodes.contains(node.id) == false)
            {
                continue;
            }

            if (candidateOffspring.insertNode(node) == false)
            {
                return false;
            }

            insertedNodes.insert(node.id);
        }

        for (const NodeGene& node : nodesB)
        {
            if (requiredNodes.contains(node.id) == false || insertedNodes.contains(node.id))
            {
                continue;
            }

            if (candidateOffspring.insertNode(node) == false)
            {
                return false;
            }

            insertedNodes.insert(node.id);
        }

        if (insertedNodes.size() != requiredNodes.size())
        {
            return false;
        }

        // Insert inherited connections.
        for (const ConnectionGene& inherited : inheritedConnections)
        {
            ConnectionGene connection = inherited;
            if (connection.enabled && candidateOffspring.wouldCreateCycle(connection.fromNode, connection.toNode))
            {
                connection.enabled = false;
            }

            if (candidateOffspring.insertConnection(connection) == false)
            {
                return false;
            }
        }

        offspring = std::move(candidateOffspring);

        return true;
    }

}
