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

        template<typename... Args> static constexpr void trace(const string& fmt, const Args&... args)     { logMessage(Type::Trace, fmt, args...); };
        template<typename... Args> static constexpr void debug(const string& fmt, const Args&... args)     { logMessage(Type::Debug, fmt, args...); };
        template<typename... Args> static constexpr void info(const string& fmt, const Args&... args)      { logMessage(Type::Info, fmt, args...); };
        template<typename... Args> static constexpr void warning(const string& fmt, const Args&... args)   { logMessage(Type::Warning, fmt, args...); };
        template<typename... Args> static constexpr void error(const string& fmt, const Args&... args)     { logMessage(Type::Error, fmt, args...); };
        template<typename... Args> static constexpr void critical(const string& fmt, const Args&... args)  { logMessage(Type::Critical, fmt, args...); };

        static LogStream trace() { return LogStream(Type::Trace); }
        static LogStream debug() { return LogStream(Type::Debug); }
        static LogStream info() { return LogStream(Type::Info); }
        static LogStream warning() { return LogStream(Type::Warning); }
        static LogStream error() { return LogStream(Type::Error); }
        static LogStream critical() { return LogStream(Type::Critical); }

    private:

        template<typename... Args>
        static void logMessage(Type type, const std::string& fmt, const Args&... args)
        {
            char buffer[1024];
            std::snprintf(buffer, sizeof(buffer), fmt.c_str(), args...);
            log(type, buffer);
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
