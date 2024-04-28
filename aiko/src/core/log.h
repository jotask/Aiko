#pragma once

#include "aiko_types.h"
#include "core/utils.h"

#include <utility>
#include <sstream>      // std::stringstream

namespace aiko
{
    class Log
    {
    private:

        #define Expand_ARGS(TYPE)                                       \
            constexpr auto concatenate = [](auto&&... xs) constexpr     \
            {                                                           \
                std::stringstream ss;                                   \
                ((ss << xs), ...);                                      \
                return ss.str();                                        \
                };                                                      \
           std::string msg = concatenate(args...);                      \
           log(TYPE, msg.c_str());                                      

    public:

        enum class Type
        {
            Trace,
            Debug,
            Info,
            Warning,
            Error,
            Critical,
        };

        template<typename... Args> static constexpr void trace(const Args&... args) { Expand_ARGS(Type::Trace) };
        template<typename... Args> static constexpr void debug(const Args&... args) { Expand_ARGS(Type::Debug) };
        template<typename... Args> static constexpr void info(const Args&... args) { Expand_ARGS(Type::Info) };
        template<typename... Args> static constexpr void warning(const Args&... args) { Expand_ARGS(Type::Warning) };
        template<typename... Args> static constexpr void error(const Args&... args) { Expand_ARGS(Type::Error) };
        template<typename... Args> static constexpr void critical(const Args&... args) { Expand_ARGS(Type::Critical) };

    private:

        static void log(Type, const char* message);

    };

}
