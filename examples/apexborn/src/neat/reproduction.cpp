
#include "reproduction.h"

#include "crossover.h"
#include "evolution_config.h"
#include "genome.h"
#include "innovation_tracker.h"
#include "math/math.h"
#include "mutation.h"
#include "species.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <random>
#include <unordered_set>
#include <utility>
#include <vector>

namespace ab
{

    bool Reproduction::reproduce(const std::vector<Genome>& genomes, std::span<const double> fitness, const std::vector<Species>& species, const EvolutionConfig& config, InnovationTracker& innovationTracker, std::mt19937_64& rng, std::vector<Genome>& offspring)
    {
        // Validate inputs.
        if (config.validate() == false)
        {
            return false;
        }

        if (genomes.size() != config.populationSize || fitness.size() != genomes.size() || species.empty())
        {
            return false;
        }

        for (const double value : fitness)
        {
            if (aiko::math::isfinite(value) == false)
            {
                return false;
            }
        }

        std::vector<bool> assigned(genomes.size(), false);
        std::unordered_set<std::uint64_t> speciesIds;

        for (const Species& currentSpecies : species)
        {
            if (currentSpecies.id == 0 || currentSpecies.members.empty() || speciesIds.insert(currentSpecies.id).second == false)
            {
                return false;
            }
            for (const size_t index : currentSpecies.members)
            {
                if (index >= genomes.size() || assigned[index])
                {
                    return false;
                }

                assigned[index] = true;
            }
        }

        if (std::all_of(assigned.begin(), assigned.end(), [](bool value) { return value; }) == false)
        {
            return false;
        }

        // Prepare reproduction data for each species.
        struct SpeciesReproductionData
        {
            std::vector<size_t> members;
            std::vector<size_t> survivors;
            long double adjustedFitness = 0.0L;
            long double remainder = 0.0L;
            size_t offspringCount = 0;
        };

        std::vector<SpeciesReproductionData> reproductionData;
        reproductionData.reserve(species.size());

        // Shift negative fitness values into a nonnegative range.
        const double minimumFitness = *std::min_element(fitness.begin(), fitness.end());
        const long double fitnessOffset = std::min(0.0L, static_cast<long double>(minimumFitness));

        for (const Species& currentSpecies : species)
        {
            SpeciesReproductionData data;
            data.members = currentSpecies.members;

            std::sort(data.members.begin(), data.members.end(),
                [&](size_t a, size_t b)
                {
                    if (fitness[a] == fitness[b])
                    {
                        return a < b;
                    }

                    return fitness[a] > fitness[b];
                });

            const long double survivorFraction = static_cast<long double>(data.members.size()) * static_cast<long double>(config.survivalFraction);
            const size_t survivorCount = std::max<size_t>( 1, static_cast<size_t>(std::ceil(survivorFraction)) );
            data.survivors.assign(data.members.begin(), data.members.begin() + survivorCount);

            long double totalFitness = 0.0L;
            for (const size_t index : data.members)
            {
                totalFitness += static_cast<long double>(fitness[index]) - fitnessOffset;
            }
            data.adjustedFitness = totalFitness / static_cast<long double>(data.members.size());
            if (std::isfinite(data.adjustedFitness) == false)
            {
                return false;
            }
            reproductionData.push_back(std::move(data));
        }

        long double totalAdjustedFitness = 0.0L;
        for (const auto& data : reproductionData)
        {
            totalAdjustedFitness += data.adjustedFitness;
        }

        if (std::isfinite(totalAdjustedFitness) == false)
        {
            return false;
        }

        size_t allocated = 0;
        for (auto& data : reproductionData)
        {
            long double proportion = 0.0L;
            if (totalAdjustedFitness > 0.0L)
            {
                proportion = data.adjustedFitness / totalAdjustedFitness;
            }
            else
            {
                proportion = 1.0L / static_cast<long double>(reproductionData.size());
            }

            const long double idealCount = proportion * static_cast<long double>(config.populationSize);
            const long double wholeCount = std::floor(idealCount);

            if (std::isfinite(idealCount) == false || wholeCount < 0.0L || wholeCount > static_cast<long double>(config.populationSize))
            {
                return false;
            }

            data.offspringCount = static_cast<size_t>(wholeCount);
            data.remainder = idealCount - wholeCount;
            allocated += data.offspringCount;

            if (allocated > config.populationSize)
            {
                return false;
            }
        }

        const size_t remaining = config.populationSize - allocated;

        std::vector<size_t> order(reproductionData.size());
        std::iota(order.begin(), order.end(), 0);

        std::sort(order.begin(), order.end(),
            [&](size_t a, size_t b)
            {
                if (reproductionData[a].remainder == reproductionData[b].remainder)
                {
                    return a < b;
                }
                return reproductionData[a].remainder > reproductionData[b].remainder;
            });

        if (remaining >= order.size())
        {
            return false;
        }

        for (size_t i = 0; i < remaining; ++i)
        {
            ++reproductionData[order[i]].offspringCount;
        }

        // Prepare atomic reproduction state.
        InnovationTracker localTracker = innovationTracker;
        std::mt19937_64 localRng = rng;

        std::vector<Genome> candidateOffspring;
        candidateOffspring.reserve(config.populationSize);

        std::bernoulli_distribution crossoverDistribution(config.crossoverProbability);
        std::bernoulli_distribution addConnectionDistribution(config.addConnectionProbability);
        std::bernoulli_distribution addNodeDistribution(config.addNodeProbability);

        // Produce offspring for each species.
        for (const SpeciesReproductionData& data : reproductionData)
        {
            if (data.offspringCount == 0)
            {
                continue;
            }

            const size_t eliteCount = std::min({config.eliteCount, data.offspringCount, data.members.size()});
            for (size_t i = 0; i < eliteCount; ++i)
            {
                candidateOffspring.push_back(genomes[data.members[i]]);
            }

            // Tournament selection from surviving parents.
            const auto selectParent = [&]() -> size_t
            {
                std::uniform_int_distribution<size_t> distribution(0, data.survivors.size() - 1);
                size_t best = data.survivors[distribution(localRng)];
                constexpr size_t TournamentSize = 3;
                for (size_t i = 1; i < TournamentSize; ++i)
                {
                    const size_t candidate = data.survivors[distribution(localRng)];
                    if (fitness[candidate] > fitness[best] || (fitness[candidate] == fitness[best] && candidate < best))
                    {
                        best = candidate;
                    }
                }
                return best;
            };

            // Produce the non-elite children.
            for (size_t i = eliteCount; i < data.offspringCount; ++i)
            {
                const size_t parentA = selectParent();
                Genome child;
                if (data.survivors.size() > 1 && crossoverDistribution(localRng))
                {
                    const size_t parentB = selectParent();
                    if (Crossover::crossover(genomes[parentA], genomes[parentB], fitness[parentA], fitness[parentB], localRng, child) == false)
                    {
                        return false;
                    }
                }
                else
                {
                    child = genomes[parentA];
                }

                Mutation::mutateWeights(child, localRng, config.weightMutationProbability, config.weightReplacementProbability, config.weightPerturbationSigma);
                if (addConnectionDistribution(localRng))
                {
                    Mutation::addConnection(child, localTracker, localRng);
                }

                if (addNodeDistribution(localRng))
                {
                    Mutation::addNode(child, localTracker, localRng);
                }

                candidateOffspring.push_back(std::move(child));
            }
        }

        // Commit successful reproduction.
        if (candidateOffspring.size() != config.populationSize)
        {
            return false;
        }

        offspring = std::move(candidateOffspring);
        innovationTracker = std::move(localTracker);
        rng = std::move(localRng);

        return true;
    }

}
