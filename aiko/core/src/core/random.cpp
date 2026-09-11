#include "random.h"

#if defined(AIKO_WINDOWS)
    #include <windows.h>
#elif defined(AIKO_LINUX)
    #include <limits.h>
    #include <unistd.h>
#else
    #error OS unsupported!
#endif

#include <random>
#include <algorithm>

namespace aiko
{
    namespace utils
    {

        std::mt19937* getRandomDevice()
        {
            static bool init = false;
            uint seed = RND_SEED;
            if (init == false)
            {
                init = true;
                if (RND_RANDOM_SEED)
                {
                    static std::random_device rd;     // Only used once to initialise (seed) engine
                    seed = rd();
                }
            }
            static std::mt19937 rng(seed);    // Random-number engine used (Mersenne-Twister in this case
            return &rng;
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

        bool getRandomBool()
        {
            std::uniform_int_distribution<int> distribution(0, 1);
            return distribution(*getRandomDevice()) == 1;
        }

    }
}