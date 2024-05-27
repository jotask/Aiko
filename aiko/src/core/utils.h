#pragma once

#include <string>
#include "aiko_types.h"

namespace aiko
{
    namespace utils
    {

        float map(float input, float input_start, float input_end, float output_start, float output_end);

        int getRandomValue(int min, int max);
        float getRandomValue(float min, float max);

        const char* join(const char*, const char*);

        const std::string generateRandomString(int length = 10);

        std::string getExePath();

    }
}
