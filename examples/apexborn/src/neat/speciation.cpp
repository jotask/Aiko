#include "speciation.h"

#include "compatibility_distance.h"
#include "math/math.h"
#include "species.h"

#include <limits>

namespace ab
{

    bool Speciation::assign(const std::vector<Genome>& genomes, double compatibilityThreshold, double excessCoefficient, double disjointCoefficient, double weightCoefficient, std::vector<Species>& species, std::uint64_t& nextSpeciesId)
    {

        if (compatibilityThreshold < 0.0 || aiko::math::isfinite(compatibilityThreshold) == false)
        {
            return false;
        }
        if (excessCoefficient < 0.0 || aiko::math::isfinite(excessCoefficient) == false)
        {
            return false;
        }
        if (disjointCoefficient < 0.0 || aiko::math::isfinite(disjointCoefficient) == false)
        {
            return false;
        }
        if (weightCoefficient < 0.0 || aiko::math::isfinite(weightCoefficient) == false)
        {
            return false;
        }
        std::vector<Species> localSpecies = species;
        for (Species& specie : localSpecies)
        {
            specie.members.clear();
        }

        std::uint64_t nextLocalSpeciesId = nextSpeciesId;

        for (std::size_t genomeIndex = 0; genomeIndex < genomes.size(); ++genomeIndex)
        {
            const Genome& genome = genomes[genomeIndex];

            bool assigned = false;

            // Try existing species.
            for (Species& currentSpecies : localSpecies)
            {
                const auto distance = CompatibilityDistance::calculate(genome, currentSpecies.representative, excessCoefficient, disjointCoefficient, weightCoefficient);

                if (distance.has_value() == false)
                {
                    return false;
                }

                if (*distance < compatibilityThreshold)
                {
                    currentSpecies.members.push_back(genomeIndex);
                    assigned = true;
                    break;
                }
            }

            // No compatible species found: create a new one.
            if (assigned == false)
            {
                if (nextLocalSpeciesId == 0 || nextLocalSpeciesId == std::numeric_limits<std::uint64_t>::max())
                {
                    return false;
                }
                Species newSpecies;
                newSpecies.id = nextLocalSpeciesId++;
                newSpecies.representative = genome;
                newSpecies.members.push_back(genomeIndex);

                localSpecies.push_back(std::move(newSpecies));
            }
        }

        std::erase_if(localSpecies, [](const Species& currentSpecies)
        {
            return currentSpecies.members.empty();
        });

        for (Species& currentSpecies : localSpecies)
        {
            currentSpecies.representative = genomes[currentSpecies.members.front()];
        }

        species = std::move(localSpecies);
        nextSpeciesId = nextLocalSpeciesId;

        return true;
    }
}
