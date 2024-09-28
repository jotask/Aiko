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
        friend class Aiko;
        static void init();

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

        static const char* endl;

        class LogStream
        {
        public:
            LogStream(Type type) : m_type(type) {}

            ~LogStream()
            {
                Log::log(m_type, m_stream.str().c_str());
            }

            // Overload the stream operator to accept various types of input
            template<typename T>
            LogStream& operator<<(const T& value)
            {
                m_stream << value;
                return *this;
            }


        private:
            Type m_type;
            std::stringstream m_stream;  // buffer for the message
        };

        template<typename... Args> static constexpr void trace(const Args&... args)     { logMessage(Type::Critical, args...); };
        template<typename... Args> static constexpr void debug(const Args&... args)     { logMessage(Type::Critical, args...); };
        template<typename... Args> static constexpr void info(const Args&... args)      { logMessage(Type::Critical, args...); };
        template<typename... Args> static constexpr void warning(const Args&... args)   { logMessage(Type::Critical, args...); };
        template<typename... Args> static constexpr void error(const Args&... args)     { logMessage(Type::Critical, args...); };
        template<typename... Args> static constexpr void critical(const Args&... args)  { logMessage(Type::Critical, args...); };

        static LogStream trace() { return LogStream(Type::Trace); }
        static LogStream debug() { return LogStream(Type::Debug); }
        static LogStream info() { return LogStream(Type::Info); }
        static LogStream warning() { return LogStream(Type::Warning); }
        static LogStream error() { return LogStream(Type::Error); }
        static LogStream critical() { return LogStream(Type::Critical); }

    private:

        template<typename... Args>
        static void logMessage(Type type, const Args&... args)
        {
            std::stringstream ss;
            (ss << ... << args);
            log(type, ss.str().c_str());
        }

        static void log(Type, const char* message);

    };

    class LogTimer
    {
    public:
        static void startTimer();
        static void endTimer();
    };

}
