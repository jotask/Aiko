#include "utils.h"

#include "core/libs.h"

#ifdef AIKO_BUILD_RAYLIB
#include <raylib.h>
#endif

namespace aiko
{
    namespace utils
    {

        float map(float input, float input_start, float input_end, float output_start, float output_end)
        {
            return output_start + ((output_end - output_start) / (input_end - input_start)) * (input - input_start);
        }

        int getRandomValue(int min, int max)
        {
            return ::GetRandomValue(min, max);
        }

        float getRandomValue(float min, float max)
        {
            return ::GetRandomValue(min, max);
        }

    }
}
