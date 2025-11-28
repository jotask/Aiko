#pragma once


#include "aiko_types.h"

namespace aiko
{
    namespace utils
    {

        constexpr bool RND_RANDOM_SEED = true;
        constexpr uint RND_SEED = 0;

        float map(float input, float input_start, float input_end, float output_start, float output_end);

        int getRandomValue(int min, int max);
        float getRandomValue(float min, float max);
        bool getRandomBool();

        const char* join(const char*, const char*);

        const string generateRandomString(int length = 10);

        string getExePath();

    }
}
