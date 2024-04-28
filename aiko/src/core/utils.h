#pragma once

#include "aiko_types.h"

namespace aiko
{
    namespace utils
    {

        float map(float input, float input_start, float input_end, float output_start, float output_end);

        int getRandomValue(int min, int max);

        const char* join(const char*, const char*);

    }
}
