#pragma once

#include <logger/logger.h>

#ifdef AIKO_PROFILER

    #if defined(_MSC_VER)
       #define AikoTracyFunction __FUNCSIG__
   #elif defined(__clang__) || defined(__GNUC__)
       #define AikoTracyFunction __PRETTY_FUNCTION__
   #endif

    #include <tracy/Tracy.hpp>
    #include <common/TracyVersion.hpp>
#endif

#ifdef AIKO_PROFILER
    #define AIKO_ZONE_SCOPED ZoneScoped;
    #define AIKO_ZONE_NAMED(name) ZoneScopedN(name)
    #define AIKO_FRAME_MARK FrameMark;
    #define AIKO_PROFILER_VERSION_PRINT aiko::logger::Log::trace( "[TRACY] v%d.%d.%d", tracy::Version::Major, tracy::Version::Minor, tracy::Version::Patch);

    #define AIKO_FUNCTION_PROFILE ZoneScopedN(AikoTracyFunction)
#else
    #define AIKO_ZONE_SCOPED
    #define AIKO_ZONE_NAMED(name)
    #define AIKO_FRAME_MARK
    #define AIKO_PROFILER_VERSION_PRINT

    #define AIKO_FUNCTION_PROFILE
#endif
