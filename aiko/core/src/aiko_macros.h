#pragma once

#include "aiko_types.h"

#include <cstdio>
#include <cstdlib>
#include <source_location>
#include <string_view>

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

    namespace detail
    {
        [[noreturn]] inline void assertionFailed(const char* condition, std::string_view message, const std::source_location location = std::source_location::current())
        {
            std::fprintf(
                stderr,
                "[ASSERT] %.*s\n"
                "  Condition: %s\n"
                "  Location: %s:%u:%u\n"
                "  Function: %s\n",
                static_cast<int>(message.size()),
                message.data(),
                condition,
                location.file_name(),
                location.line(),
                location.column(),
                location.function_name()
            );

            std::fflush(stderr);
            std::abort();
        }

        inline void todo(std::string_view message, const std::source_location location = std::source_location::current())
        {
            std::fprintf(
                stderr,
                "[TODO] %.*s\n"
                "  Location: %s:%u:%u\n"
                "  Function: %s\n",
                static_cast<int>(message.size()),
                message.data(),
                location.file_name(),
                location.line(),
                location.column(),
                location.function_name()
            );

            std::fflush(stderr);
        }
    }

    #define AIKO_ASSERT(cond, msg)                                                      \
        do                                                                              \
        {                                                                               \
            if (!(cond))                                                                \
            {                                                                           \
                ::aiko::detail::assertionFailed(#cond, (msg));                          \
            }                                                                           \
        } while (false)

    #define AIKO_ASSERTF(cond, fmt, ...)                                                \
            do                                                                          \
            {                                                                           \
                if (!(cond))                                                            \
                {                                                                       \
                    logger::Log::error(fmt __VA_OPT__(,) __VA_ARGS__);                  \
                    ::aiko::detail::assertionFailed(                                    \
                        #cond,                                                          \
                        "Formatted assertion failed"                                    \
                    );                                                                  \
                }                                                                       \
            } while (false)


    #define AIKO_TODO(msg) ::aiko::detail::todo((msg))

    #define AIKO_STRICT_NOT_IMPLEMENTED false

    #if AIKO_STRICT_NOT_IMPLEMENTED
        #define AIKO_NOT_IMPLEMENTED static_assert(false, "NOT IMPLEMENTED")
    #else
        #define AIKO_NOT_IMPLEMENTED                                               \
            do                                                                     \
            {                                                                      \
                AIKO_TODO("NOT IMPLEMENTED");                                      \
                AIKO_DEBUG_BREAK;                                                  \
            } while (0)
    #endif

    #define AIKO_UNUSED(var) (void)var;
    
}
