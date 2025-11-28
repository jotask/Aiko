#pragma once

#include <memory>
#include <string>
#include <assert.h>
#include <stdint.h>
#include <stdexcept>

#if defined(__cplusplus)
#define CLITERAL(type)      type
#else
#define CLITERAL(type)      (type)
#endif

namespace aiko
{

    using uint = unsigned int;
    using u8 = uint8_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using string = std::string;

    template<class T>
    using AikoPtr = std::shared_ptr<T>;

    template<class T>
    using AikoUPtr = std::unique_ptr<T>;

    #define AIKO_NOT_IMPLEMENTED static_assert(true, "NOT IMPLEMENTED");
    #if defined(_MSC_VER)
    #define AIKO_DEBUG_BREAK __debugbreak()
    #elif defined(__GNUC__) || defined(__clang__)
    #define AIKO_DEBUG_BREAK __builtin_trap();
    #else
    #define AIKO_DEBUG_BREAK ((void)0)
    #endif
    #define AIKO_ASSERT(cond, msg) assert(cond && msg);

    template<typename ... Args>
    string string_format(const string& format, Args ... args)
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
        if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
        auto size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, format.c_str(), args ...);
        return string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
    }

}
