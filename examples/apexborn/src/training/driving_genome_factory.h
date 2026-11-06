#pragma once

#include <optional>

namespace ab
{

    class Genome;

    class DrivingGenomeFactory final
    {
    public:
        static std::optional<Genome> createSeedGenome(size_t input, size_t output);
    };

}
