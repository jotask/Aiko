#include "compatibility_distance.h"

#include "genome.h"
#include "math/math.h"

#include <cmath>
#include <algorithm>

namespace ab
{

    std::optional<double> CompatibilityDistance::calculate(const Genome& genomeA, const Genome& genomeB, double excessCoefficient, double disjointCoefficient, double weightCoefficient)
    {

        if (excessCoefficient < 0.0 || aiko::math::isfinite(excessCoefficient) == false)
        {
            return std::nullopt;
        }
        if (disjointCoefficient < 0.0 || aiko::math::isfinite(disjointCoefficient) == false)
        {
            return std::nullopt;
        }
        if (weightCoefficient < 0.0 || aiko::math::isfinite(weightCoefficient) == false)
        {
            return std::nullopt;
        }

        std::size_t indexA = 0;
        std::size_t indexB = 0;
        std::size_t excessCount = 0;
        std::size_t disjointCount = 0;
        std::size_t matchingCount = 0;
        double totalWeightDifference = 0.0;

        const auto& connectionsA = genomeA.getConnections();
        const auto& connectionsB = genomeB.getConnections();

        const InnovationId maxInnovationA = connectionsA.empty() ? InvalidInnovationId : connectionsA.back().innovationId;
        const InnovationId maxInnovationB = connectionsB.empty() ? InvalidInnovationId : connectionsB.back().innovationId;

        while (indexA < connectionsA.size() && indexB < connectionsB.size())
        {
            const InnovationId innovationA = connectionsA[indexA].innovationId;
            const InnovationId innovationB = connectionsB[indexB].innovationId;
            if (innovationA == innovationB)
            {
                const ConnectionGene& connectionA = connectionsA[indexA];
                const ConnectionGene& connectionB = connectionsB[indexB];

                if (connectionA.fromNode != connectionB.fromNode || connectionA.toNode != connectionB.toNode)
                {
                    return std::nullopt;
                }

                totalWeightDifference += std::abs(connectionA.weight - connectionB.weight);

                ++matchingCount;
                ++indexA;
                ++indexB;
            }
            else if (innovationA < innovationB)
            {
                if (innovationA > maxInnovationB)
                {
                    ++excessCount;
                }
                else
                {
                    ++disjointCount;
                }
                ++indexA;
            }
            else
            {
                if (innovationB > maxInnovationA)
                {
                    ++excessCount;
                }
                else
                {
                    ++disjointCount;
                }
                ++indexB;
            }
        }

        while (indexA < connectionsA.size())
        {
            if (connectionsA[indexA].innovationId > maxInnovationB)
            {
                ++excessCount;
            }
            else
            {
                ++disjointCount;
            }
            ++indexA;
        }

        while (indexB < connectionsB.size())
        {
            if (connectionsB[indexB].innovationId > maxInnovationA)
            {
                ++excessCount;
            }
            else
            {
                ++disjointCount;
            }
            ++indexB;
        }

        const std::size_t N = std::max<std::size_t>(1, std::max(connectionsA.size(), connectionsB.size()));

        const double averageWeightDifference = matchingCount > 0 ? totalWeightDifference / static_cast<double>(matchingCount) : 0.0;

        const double distance =
            excessCoefficient * static_cast<double>(excessCount) / static_cast<double>(N) +
            disjointCoefficient * static_cast<double>(disjointCount) / static_cast<double>(N) +
            weightCoefficient * averageWeightDifference;

        if (aiko::math::isfinite(distance) == false)
        {
            return std::nullopt;
        }

        return distance;

    }

}
