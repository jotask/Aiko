#include "asset_type.h"

#include <random>

namespace aiko::asset
{

    ID Asset::generateID()
    {
        std::random_device rd;
        std::mt19937_64 gen(rd()); // Generator initialized with random_device
        std::uniform_int_distribution<ID> dis;
        return dis(gen); // Generate a random 64-bit number
    }

    Asset::Asset()
        : m_id(generateID())
    {

    }

}
