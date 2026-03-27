#pragma once

#ifdef AIKO_PROFILER
    #include <source_location>
    #include <tracy/Tracy.hpp>
#endif

#ifdef AIKO_PROFILER
    #define AIKO_ZONE_SCOPED ZoneScoped;
    #define AIKO_ZONE_NAMED(name) ZoneScoped(name)
    #define AIKO_FRAME_MARK FrameMark;
#else
    #define AIKO_ZONE_SCOPED
    #define AIKO_ZONE_NAMED(name)
    #define AIKO_FRAME_MARK
#endif
