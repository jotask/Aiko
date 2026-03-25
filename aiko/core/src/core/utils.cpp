#include "utils.h"

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

        float map(float input, float input_start, float input_end, float output_start, float output_end)
        {
            return output_start + ((output_end - output_start) / (input_end - input_start)) * (input - input_start);
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
            #if defined (AIKO_WINDOWS)
            char result[MAX_PATH];
            return string(result, GetModuleFileName(NULL, result, MAX_PATH));
            #elif defined(AIKO_LINUX)
            char result[PATH_MAX];
            ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
            return string(result, (count > 0) ? count : 0);
            #else
            #error OS unsupported!
            #endif
        }

        void hashCombine(const std::size_t value, std::size_t& seed)
        {
                seed ^= value + 0x9e3779b97f4a7c15ull + (seed << 6) + (seed >> 2);
        }
    }
}