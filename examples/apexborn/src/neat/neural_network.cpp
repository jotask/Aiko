#include "neural_network.h"

#include "genome.h"
#include "math/math.h"

#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <utility>

namespace ab
{
    bool NeuralNetwork::build(const Genome& genome)
    {
        m_valid = false;
        m_nodes.clear();
        m_executionOrder.clear();
        m_inputIndices.clear();
        m_outputIndices.clear();

        const auto& nodes = genome.getNodes();
        const auto& connections = genome.getConnections();

        if (nodes.empty())
        {
            return false;
        }

        // 1. Create runtime nodes and map NodeId -> runtime index.
        std::vector<RuntimeNode> runtimeNodes;
        runtimeNodes.reserve(nodes.size());

        std::unordered_map<NodeId, size_t> nodeIndices;
        nodeIndices.reserve(nodes.size());

        std::vector<std::pair<NodeId, size_t>> inputIndices;
        std::vector<std::pair<NodeId, size_t>> outputIndices;

        for (const NodeGene& gene : nodes)
        {
            const size_t index = runtimeNodes.size();

            if (nodeIndices.emplace(gene.id, index).second == false)
            {
                return false;
            }

            runtimeNodes.push_back(RuntimeNode{
                .id = gene.id,
                .type = gene.type,
                .incomingConnections = {}
            });

            if (gene.type == NodeType::Input)
            {
                inputIndices.emplace_back(gene.id, index);
            }
            else if (gene.type == NodeType::Output)
            {
                outputIndices.emplace_back(gene.id, index);
            }
        }

        if (inputIndices.empty() || outputIndices.empty())
        {
            return false;
        }

        // 2. Resolve enabled connections.
        // Outgoing indices are only needed during compilation.
        std::vector<std::vector<size_t>> outgoingIndices(runtimeNodes.size());
        std::vector<size_t> inDegrees(runtimeNodes.size(), 0);

        for (const ConnectionGene& connection : connections)
        {
            if (connection.enabled == false)
            {
                continue;
            }

            if (aiko::math::isfinite(connection.weight) == false)
            {
                return false;
            }

            const auto from = nodeIndices.find(connection.fromNode);
            const auto to = nodeIndices.find(connection.toNode);

            if (from == nodeIndices.end() || to == nodeIndices.end())
            {
                return false;
            }

            const size_t fromIndex = from->second;
            const size_t toIndex = to->second;

            runtimeNodes[toIndex].incomingConnections.push_back(
                RuntimeConnection{
                    .sourceIndex = fromIndex,
                    .weight = connection.weight
                }
            );

            outgoingIndices[fromIndex].push_back(toIndex);
            ++inDegrees[toIndex];
        }

        // 3. Topologically sort the runtime nodes.
        std::queue<size_t> readyNodes;
        for (size_t i = 0; i < inDegrees.size(); ++i)
        {
            if (inDegrees[i] == 0)
            {
                readyNodes.push(i);
            }
        }

        std::vector<size_t> executionOrder;
        executionOrder.reserve(runtimeNodes.size());

        while (readyNodes.empty() == false)
        {
            const size_t currentIndex = readyNodes.front();
            readyNodes.pop();

            executionOrder.push_back(currentIndex);

            for (const size_t destinationIndex : outgoingIndices[currentIndex])
            {
                --inDegrees[destinationIndex];

                if (inDegrees[destinationIndex] == 0)
                {
                    readyNodes.push(destinationIndex);
                }
            }
        }

        // Not all nodes processed means the graph contains a cycle.
        if (executionOrder.size() != runtimeNodes.size())
        {
            return false;
        }

        // 4. Establish deterministic external input/output ordering.
        const auto byNodeId = [](const auto& a, const auto& b)
        {
            return a.first < b.first;
        };

        std::sort(inputIndices.begin(), inputIndices.end(), byNodeId);
        std::sort(outputIndices.begin(), outputIndices.end(), byNodeId);

        std::vector<size_t> compiledInputIndices;
        std::vector<size_t> compiledOutputIndices;

        compiledInputIndices.reserve(inputIndices.size());
        compiledOutputIndices.reserve(outputIndices.size());

        for (const auto& [id, index] : inputIndices)
        {
            compiledInputIndices.push_back(index);
        }

        for (const auto& [id, index] : outputIndices)
        {
            compiledOutputIndices.push_back(index);
        }

        // 5. Commit the successfully compiled network.
        m_nodes = std::move(runtimeNodes);
        m_executionOrder = std::move(executionOrder);
        m_inputIndices = std::move(compiledInputIndices);
        m_outputIndices = std::move(compiledOutputIndices);

        m_valid = true;
        return true;
    }

    bool NeuralNetwork::evaluate(std::span<const double> inputs, std::span<double> outputs, std::vector<NodeActivation>* activations) const
    {
        if (m_valid == false)
        {
            return false;
        }
        if (inputs.size() != m_inputIndices.size())
        {
            return false;
        }
        if (outputs.size() != m_outputIndices.size())
        {
            return false;
        }

        std::vector<double> neuronValues(m_nodes.size(), 0.0);

        for (size_t i = 0; i < m_inputIndices.size(); ++i)
        {
            const auto input = inputs[i];
            if (aiko::math::isfinite(input) == false)
            {
                return false;
            }
            const auto runtimeIndex = m_inputIndices[i];
            neuronValues[runtimeIndex] = input;
        }

        for (size_t i = 0; i < m_nodes.size(); ++i)
        {
            const auto& node = m_nodes[i];
            if (node.type == NodeType::Bias)
            {
                neuronValues[i] = 1.0;
            }
        }

        for (const auto& nodeIndex : m_executionOrder)
        {
            const RuntimeNode& node = m_nodes[nodeIndex];
            if (node.type == NodeType::Input || node.type == NodeType::Bias)
            {
                continue;
            }
            double weightSum = 0.0;
            for (const RuntimeConnection& connection : node.incomingConnections)
            {
                const auto contribution = neuronValues[connection.sourceIndex] * connection.weight;
                weightSum += contribution;
            }
            if (aiko::math::isfinite(weightSum) == false)
            {
                return false;
            }
            const auto activation = std::tanh(weightSum);
            if (aiko::math::isfinite(activation) == false)
            {
                return false;
            }
            neuronValues[nodeIndex] = activation;
        }

        for (size_t i = 0 ; i < m_outputIndices.size(); ++i)
        {
            const auto runtimeNodeIdx = m_outputIndices[i];
            const auto neuronValue = neuronValues[runtimeNodeIdx];
            if (aiko::math::isfinite(neuronValue) == false)
            {
                return false;
            }
        }

        for (size_t i = 0 ; i < m_outputIndices.size(); ++i)
        {
            const auto runtimeNodeIdx = m_outputIndices[i];
            const auto value = neuronValues[runtimeNodeIdx];
            outputs[i] = value;
        }

        if (activations != nullptr)
        {
            std::vector<NodeActivation> captured;
            captured.reserve(m_nodes.size());
            for (size_t i = 0; i < m_nodes.size(); ++i)
            {
                captured.push_back(NodeActivation{
                    .id = m_nodes[i].id,
                    .value = neuronValues[i]
                });
            }
            *activations = std::move(captured);
        }

        return true;
    }
}
