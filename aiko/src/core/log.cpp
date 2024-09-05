#include "log.h"

#include <chrono>
#include <iostream>
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <sstream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace aiko
{
    void Log::init()
    {
        // Create a color console logger
        auto console = spdlog::stdout_color_mt("console");
        spdlog::set_level(spdlog::level::level_enum::trace);
    }

    void Log::log(Type type, const char* msg)
    {
        switch (type)
        {
            case aiko::Log::Type::Trace:        spdlog::trace(msg);    return;
            case aiko::Log::Type::Debug:        spdlog::debug(msg);    return;
            case aiko::Log::Type::Info:         spdlog::info(msg);     return;
            case aiko::Log::Type::Warning:      spdlog::warn(msg);     return;
            case aiko::Log::Type::Error:        spdlog::error(msg);    return;
            case aiko::Log::Type::Critical:     spdlog::critical(msg); return;
            default:
                break;
        }
    }

    static std::chrono::time_point<std::chrono::high_resolution_clock> m_start = std::chrono::high_resolution_clock::now();

    void LogTimer::startTimer()
    {
        m_start = std::chrono::high_resolution_clock::now();
    }

    void LogTimer::endTimer()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start).count();
        Log::trace("Function took " , duration , " ms to execute.");
    }

}
