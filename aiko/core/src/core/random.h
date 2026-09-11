#pragma once

#include "aiko_types.h"

namespace aiko
{
    namespace utils
    {

        constexpr bool RND_RANDOM_SEED = true;
        constexpr uint RND_SEED = 0;

        int getRandomValue(int min, int max);
        float getRandomValue(float min, float max);
        bool getRandomBool();

    }
}
