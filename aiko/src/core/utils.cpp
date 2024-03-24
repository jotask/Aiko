#include "utils.h"

#include <random>

#include "core/libs.h"

namespace aiko
{
    namespace utils
    {

        float map(float input, float input_start, float input_end, float output_start, float output_end)
        {
            return output_start + ((output_end - output_start) / (input_end - input_start)) * (input - input_start);
        }

        std::mt19937* getRandomDevice()
        {
            static std::random_device rd;     // Only used once to initialise (seed) engine
            static std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case
            return &rng;
        }

        template<class T>
        T getRandom(T min, T max)
        {
            std::uniform_int_distribution<T> uni(min, max);
            return uni(*getRandomDevice());
        }

        int getRandomValue(int min, int max)
        {
            std::uniform_int_distribution<int> uni(min, max);
            return uni(*getRandomDevice());
        }

        float getRandomValue(float min, float max)
        {
            std::uniform_real_distribution<float> uni(min, max);
            return uni(*getRandomDevice());
        }

    }
}
