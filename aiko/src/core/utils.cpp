#include "utils.h"

#ifdef WIN32
    #include <windows.h>
#elif LINUX
    #include <limits.h>
    #include <unistd.h>
#else
    #error OS unsupported!
#endif

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

        const char* join(const char* one, const char* two)
        {
            static string buf;
            buf.clear();
            buf.append(one);
            buf.append(two);
            return buf.c_str();
        }

        const string generateRandomString(int length)
        {
            auto randchar = []() -> char
                {
                    const char charset[] =
                        "0123456789"
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "abcdefghijklmnopqrstuvwxyz";
                    const size_t max_index = (sizeof(charset) - 1);
                    return charset[rand() % max_index];
                };
            string str(length, 0);
            std::generate_n(str.begin(), length, randchar);
            return str;
        }

        string getExePath()
        {
            #ifdef WIN32
            char result[MAX_PATH];
            return string(result, GetModuleFileName(NULL, result, MAX_PATH));
            #elif LINUX
            char result[PATH_MAX];
            ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
            return string(result, (count > 0) ? count : 0);
            #else
            #error OS unsupported!
            #endif
        }

    }
}