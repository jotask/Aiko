
#pragma once

#include "genes.h"

#include <cstddef>
#include <vector>
#include <span>

namespace ab
{
    class Genome;

    class NeuralNetwork
    {
    public:
        bool build(const Genome& genome);
        bool evaluate(std::span<const double> inputs, std::span<double> outputs) const;
        bool isValid() const { return m_valid; }

    private:
        struct RuntimeConnection
        {
            std::size_t sourceIndex;
            double weight;
        };

        struct RuntimeNode
        {
            NodeType type;
            std::vector<RuntimeConnection> incomingConnections;
        };

        std::vector<RuntimeNode> m_nodes;
        std::vector<std::size_t> m_executionOrder;
        std::vector<std::size_t> m_inputIndices;
        std::vector<std::size_t> m_outputIndices;

        bool m_valid = false;
    };
}
