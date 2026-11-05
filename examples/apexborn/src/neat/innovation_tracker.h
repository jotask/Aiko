#pragma once

#include "genes.h"

#include <map>
#include <optional>
#include <utility>

namespace ab
{

    struct SplitInnovation
    {
        NodeId hiddenNode;
        NodeId fromNode;
        NodeId toNode;
        InnovationId inInnovationId;
        InnovationId outInnovationId;
    };

    class InnovationTracker
    {
    public:

        InnovationTracker(const NodeId firstAvailableNodeId);

        InnovationId generateId(const NodeId from, const NodeId to);
        std::optional<SplitInnovation> generateSplitInnovation(const InnovationId originalInnovationId, const NodeId from, const NodeId to);

        void clearGenerationHistory();

    private:

        using ConnectionKey = std::pair<NodeId, NodeId>;

        std::map<ConnectionKey, InnovationId> m_innovationIds;
        std::map<InnovationId, SplitInnovation> m_splitInnovations;

        InnovationId m_nextAvailableInnovationId = InvalidInnovationId + 1;
        NodeId m_nextAvailableNodeId;

    };

}
