#pragma once

#ifdef AIKO_PROFILER
    #include <tracy/Tracy.hpp>
#endif

namespace aiko
{

    #ifdef AIKO_PROFILER
        #define AIKO_ZONE_SCOPED ZoneScoped(__FUNCTION__);
        #define AIKO_FRAME_MARK FrameMark;
    #else
        #define AIKO_ZONE_SCOPED
        #define AIKO_FRAME_MARK
    #endif

}