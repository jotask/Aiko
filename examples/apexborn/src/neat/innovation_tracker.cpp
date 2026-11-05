#include "innovation_tracker.h"

#include "aiko_macros.h"

#include <limits>

namespace ab
{

    InnovationTracker::InnovationTracker(const NodeId firstAvailableNodeId, InnovationId firstAvailableInnovationId)
        : m_nextAvailableNodeId(firstAvailableNodeId)
        , m_nextAvailableInnovationId(firstAvailableInnovationId)
    {
        AIKO_ASSERT(m_nextAvailableNodeId != InvalidNodeId, "First Available NodeId can't be the invalid one");
    }

    InnovationId InnovationTracker::generateId(const NodeId from, const NodeId to)
    {
        if (from == InvalidNodeId || to == InvalidNodeId)
        {
            return InvalidInnovationId;
        }
        const auto key = ConnectionKey{from, to};
        const auto found = m_innovationIds.find(key);
        if (found != m_innovationIds.end())
        {
            return found->second;
        }
        AIKO_ASSERT(m_nextAvailableInnovationId < std::numeric_limits<InnovationId>::max(), "Reaching maximum InnovationId, Overflowing");
        const auto generatedId = m_nextAvailableInnovationId++;
        m_innovationIds.insert({key, generatedId});
        return generatedId;
    }

    void InnovationTracker::clearGenerationHistory()
    {
        m_innovationIds.clear();
        m_splitInnovations.clear();
    }

    std::optional<SplitInnovation> InnovationTracker::generateSplitInnovation(const InnovationId originalInnovationId, const NodeId from, const NodeId to)
    {
        if (originalInnovationId == InvalidInnovationId)
        {
            return std::nullopt;
        }
        if (from == InvalidNodeId)
        {
            return std::nullopt;
        }
        if (to == InvalidNodeId)
        {
            return std::nullopt;
        }
        const auto found = m_splitInnovations.find(originalInnovationId);
        if (found != m_splitInnovations.end())
        {
            const auto& originalInnovation = found->second;
            if (originalInnovation.fromNode != from || originalInnovation.toNode != to)
            {
                return std::nullopt;
            }
            return found->second;
        }

        AIKO_ASSERT(m_nextAvailableNodeId < std::numeric_limits<NodeId>::max(), "Reaching maximum NodeId, Overflowing");
        AIKO_ASSERT(m_nextAvailableInnovationId < std::numeric_limits<InnovationId>::max() - 1, "Reaching maximum InnovationId, Overflowing");

        const NodeId newHiddenNode = m_nextAvailableNodeId++;
        const InnovationId fromToHidden = generateId(from, newHiddenNode);
        const InnovationId hiddenToto = generateId(newHiddenNode, to);

        const SplitInnovation newSplitInnovation = {
            .hiddenNode = newHiddenNode,
            .fromNode =  from,
            .toNode = to,
            .inInnovationId = fromToHidden,
            .outInnovationId = hiddenToto
        };
        m_splitInnovations.emplace(originalInnovationId, newSplitInnovation);
        return newSplitInnovation;

    }
}
