#include "genome.h"

#include "math/math.h"

#include <algorithm>
#include <cmath>
#include <stack>
#include <unordered_set>

namespace ab
{

    bool Genome::insertNode(const NodeGene& gene)
    {
        if(gene.id == InvalidNodeId)
        {
            return false;
        }
        if (std::find_if(m_nodes.begin(), m_nodes.end(), [&](const NodeGene& other){ return gene.id == other.id; }) != m_nodes.end())
        {
           return false;
        }
        m_nodes.push_back(gene);
        return true;
    }

    bool Genome::insertConnection(const ConnectionGene& connection)
    {
        if(connection.innovationId == InvalidInnovationId)
        {
            return false;
        }
        if(connection.fromNode == InvalidNodeId)
        {
            return false;
        }
        if(connection.toNode == InvalidNodeId)
        {
            return false;
        }
        if (std::find_if(m_nodes.begin(), m_nodes.end(), [&](const NodeGene& other){ return connection.fromNode == other.id; }) == m_nodes.end())
        {
            return false;
        }
        if (std::find_if(m_nodes.begin(), m_nodes.end(), [&](const NodeGene& other){ return connection.toNode == other.id; }) == m_nodes.end())
        {
            return false;
        }
        if (std::find_if(m_connections.begin(), m_connections.end(), [&](const ConnectionGene& other){ return connection.innovationId == other.innovationId; }) != m_connections.end())
        {
            return false;
        }
        if (hasConnection(connection.fromNode, connection.toNode) == true)
        {
            return false;
        }
        if (aiko::math::isfinite(connection.weight) == false)
        {
            return false;
        }
        if ( connection.enabled == true && wouldCreateCycle(connection.fromNode, connection.toNode) == true)
        {
            return false;
        }
        auto it = std::lower_bound(m_connections.begin(), m_connections.end(), connection, [](const ConnectionGene& lhs, const ConnectionGene& rhs){ return lhs.innovationId < rhs.innovationId; });
        m_connections.insert(it, connection);
        return true;
    }

    bool Genome::hasConnection(const NodeId from, const NodeId to) const
    {
        return std::find_if(m_connections.begin(), m_connections.end(), [&](const ConnectionGene& other){ return from == other.fromNode && to == other.toNode; }) != m_connections.end();
    }

    bool Genome::wouldCreateCycle(const NodeId from, const NodeId to) const
    {
        if (from == to)
        {
            return true;
        }

        std::stack<NodeId> stack;
        std::unordered_set<NodeId> visited;

        stack.emplace(to);
        visited.insert(to);

        while(stack.empty() == false)
        {

            const NodeId currentNode = stack.top();
            stack.pop();

            if (from == currentNode)
            {
                return true;
            }
            for (const auto& connection : m_connections)
            {
                if (connection.enabled == false)
                {
                    continue;
                }
                if (connection.fromNode == currentNode && visited.contains(connection.toNode) == false)
                {
                    visited.insert(connection.toNode);
                    stack.emplace(connection.toNode);
                }
            }
        }
        return false;
    }

    bool Genome::setConnectionEnabled(const InnovationId innovationId, bool enabled)
    {
        const auto found = std::find_if(m_connections.begin(), m_connections.end(), [&](const ConnectionGene& other){ return other.innovationId == innovationId; });
        if (found == m_connections.end())
        {
            return false;
        }
        if (enabled == true && found->enabled == false)
        {
            if (wouldCreateCycle(found->fromNode, found->toNode) == true)
            {
                return false;
            }
        }
        found->enabled = enabled;
        return true;
    }

    bool Genome::setConnectionWeight(const InnovationId innovationId, double weight)
    {
        if (aiko::math::isfinite(weight) == false)
        {
            return false;
        }
        const auto found = std::find_if(m_connections.begin(), m_connections.end(), [&](const ConnectionGene& other){ return other.innovationId == innovationId; });
        if (found == m_connections.end())
        {
            return false;
        }
        found->weight = weight;
        return true;
    }
}
