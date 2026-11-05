#pragma once

#include <cstdint>
#include <limits>

namespace ab
{

    using NodeId = std::uint64_t;
    using InnovationId = std::uint64_t;

    constexpr NodeId InvalidNodeId = std::numeric_limits<NodeId>::min();
    constexpr InnovationId InvalidInnovationId = std::numeric_limits<InnovationId>::min();

    enum class NodeType
    {
        Input,
        Hidden,
        Output,
        Bias
    };

    struct NodeGene
    {
        NodeId id = InvalidNodeId;
        NodeType type = NodeType::Input;
    };

    struct ConnectionGene
    {
        InnovationId innovationId = InvalidInnovationId;
        NodeId fromNode = InvalidNodeId;
        NodeId toNode = InvalidNodeId;
        double weight = 0.0;
        bool enabled = true;
    };

}
