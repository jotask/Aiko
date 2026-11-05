#pragma once

#include "math/math.h"

#include <cstddef>
#include <cstdint>

namespace ab
{
    struct EvolutionConfig
    {
        size_t populationSize = 150;
        double compatibilityThreshold = 3.0;
        double excessCoefficient = 1.0;
        double disjointCoefficient = 1.0;
        double weightCoefficient = 0.4;
        double survivalFraction = 0.5;
        double crossoverProbability = 0.75;
        double weightMutationProbability = 0.8;
        double weightReplacementProbability = 0.1;
        double weightPerturbationSigma = 0.2;
        double addConnectionProbability = 0.05;
        double addNodeProbability = 0.03;
        size_t eliteCount = 1;

        bool validate() const
        {
            const auto validProbability = [](double value)
            {
                return aiko::math::isfinite(value) && value >= 0.0 && value <= 1.0;
            };

            if (populationSize == 0)
            {
                return false;
            }

            if (aiko::math::isfinite(compatibilityThreshold) == false || compatibilityThreshold <= 0.0)
            {
                return false;
            }

            if (aiko::math::isfinite(excessCoefficient) == false || excessCoefficient < 0.0 ||
                aiko::math::isfinite(disjointCoefficient) == false || disjointCoefficient < 0.0 ||
                aiko::math::isfinite(weightCoefficient) == false || weightCoefficient < 0.0)
            {
                return false;
            }

            if (validProbability(survivalFraction) == false || survivalFraction == 0.0)
            {
                return false;
            }

            if (validProbability(crossoverProbability) == false ||
                validProbability(weightMutationProbability) == false ||
                validProbability(weightReplacementProbability) == false ||
                validProbability(addConnectionProbability) == false ||
                validProbability(addNodeProbability) == false)
            {
                return false;
            }

            if (aiko::math::isfinite(weightPerturbationSigma) == false || weightPerturbationSigma < 0.0)
            {
                return false;
            }

            return true;
        }
    };
}
