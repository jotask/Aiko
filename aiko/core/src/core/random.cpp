#include "random.h"

#include <mutex>
#include <random>

namespace aiko::utils
{
    namespace
    {
        std::mutex& getRandomMutex()
        {
            static std::mutex mutex;
            return mutex;
        }

        // Caller must hold getRandomMutex().
        std::mt19937& getRandomDevice()
        {
            static std::mt19937 generator(
                []
                {
                    if (RND_RANDOM_SEED)
                    {
                        std::random_device device;
                        return device();
                    }

                    return RND_SEED;
                }());

            return generator;
        }
    }

    int getRandomValue(int min, int max)
    {
        std::scoped_lock lock(getRandomMutex());
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(getRandomDevice());
    }

    float getRandomValue(float min, float max)
    {
        std::scoped_lock lock(getRandomMutex());
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(getRandomDevice());
    }

    bool getRandomBool()
    {
        std::scoped_lock lock(getRandomMutex());
        std::uniform_int_distribution<int> distribution(0, 1);
        return distribution(getRandomDevice()) == 1;
    }
}
