#pragma once

#include "aiko_types.h"

namespace aiko
{
    namespace utils
    {

        float map(float input, float input_start, float input_end, float output_start, float output_end);

        const char* join(const char*, const char*);

        const string generateRandomString(int length = 10);

        string getExePath();

        void hashCombine(const std::size_t value, std::size_t& seed );

    }
}
