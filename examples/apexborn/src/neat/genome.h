#pragma once

#include "genes.h"

#include <vector>

namespace ab
{

    class Genome
    {
    public:

        bool insertNode(const NodeGene& gene);
        bool insertConnection(const ConnectionGene& connection);

        bool hasConnection(const NodeId from, const NodeId to) const;
        bool wouldCreateCycle(const NodeId from, const NodeId to) const;

        bool setConnectionEnabled(const InnovationId innovationId, bool enabled);
        bool setConnectionWeight(const InnovationId innovationId, double weight);

        const auto& getNodes() const { return m_nodes; }
        const auto& getConnections() const { return m_connections; }

    private:
        std::vector<NodeGene> m_nodes;
        std::vector<ConnectionGene> m_connections;

    };

}
