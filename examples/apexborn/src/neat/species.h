#pragma once

#include "genome.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace ab
{

    struct Species
    {
        std::uint64_t id = 0;
        Genome representative;
        std::vector<size_t> members;
    };
}
