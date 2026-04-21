#pragma once

#include "aiko_types.h"

#include <source_location>
#include <format>

#if defined(__cplusplus)
#define CLITERAL(type)      type
#else
#define CLITERAL(type)      (type)
#endif

namespace aiko
{

    #if defined(_MSC_VER)
    #define AIKO_DEBUG_BREAK __debugbreak()
    #elif defined(__GNUC__) || defined(__clang__)
    #define AIKO_DEBUG_BREAK __builtin_trap();
    #else
    #define AIKO_DEBUG_BREAK ((void)0)
    #endif

    #define AIKO_ASSERT(cond, msg)                                                      \
        do {                                                                            \
            if (!(cond)) {                                                              \
                const std::source_location loc = std::source_location::current();       \
                const std::string str = std::format(                                    \
                    "[ASSERT]::{}::{}::{}::{}",                                         \
                    loc.file_name(),                                                    \
                    loc.line(),                                                         \
                    loc.column(),                                                       \
                    loc.function_name()                                                 \
                    );                                                                  \
                assert(cond && str.c_str());                                            \
            }                                                                           \
        } while (false);

    #define AIKO_ASSERTF(cond, fmt, ...)                                                \
            do {                                                                        \
                if (!(cond)) {                                                          \
                    logger::Log::error(fmt, __VA_ARGS__);                               \
                    assert(cond);                                                       \
                }                                                                       \
            } while (false);


    // TODO the TODO
    #define AIKO_TODO(msg)                                                              \
        do                                                                              \
        {                                                                               \
            printf("%s::%d::%s::[TODO] %s\n", __FILE__, __LINE__, __FUNCTION__, msg);   \
        }                                                                               \
        while (false);                                                                      \

    #define AIKO_STRICT_NOT_IMPLEMENTED false

    #if AIKO_STRICT_NOT_IMPLEMENTED
        #define AIKO_NOT_IMPLEMENTED static_assert(false, "NOT IMPLEMENTED")
    #else
        #define AIKO_NOT_IMPLEMENTED                                               \
            do                                                                     \
            {                                                                      \
                AIKO_TODO("NOT IMPLEMENTED");                                      \
                AIKO_DEBUG_BREAK;                                                        \
            } while (0)
    #endif

    #define AIKO_UNUSED(var) (void)var;
    
}
