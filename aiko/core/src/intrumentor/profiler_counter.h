#pragma once

#include <atomic>
#include <cstdint>

namespace aiko::profile
{
    struct CopyMoveCounters
    {
        std::atomic<uint64_t> copyCtor{0};
        std::atomic<uint64_t> moveCtor{0};
        std::atomic<uint64_t> copyAssign{0};
        std::atomic<uint64_t> moveAssign{0};

        void reset()
        {
            copyCtor.store(0, std::memory_order_relaxed);
            moveCtor.store(0, std::memory_order_relaxed);
            copyAssign.store(0, std::memory_order_relaxed);
            moveAssign.store(0, std::memory_order_relaxed);
        }
    };
}

#if defined(AIKO_PROFILE_COPIES)

    #define AIKO_DECLARE_COPY_MOVE_COUNTERS() \
        inline static aiko::profile::CopyMoveCounters s_counters{}; \
        static void resetCounters() { s_counters.reset(); }

    #define AIKO_COUNT_COPY_CTOR() \
        s_counters.copyCtor.fetch_add(1, std::memory_order_relaxed)

    #define AIKO_COUNT_MOVE_CTOR() \
        s_counters.moveCtor.fetch_add(1, std::memory_order_relaxed)

    #define AIKO_COUNT_COPY_ASSIGN() \
        s_counters.copyAssign.fetch_add(1, std::memory_order_relaxed)

    #define AIKO_COUNT_MOVE_ASSIGN() \
        s_counters.moveAssign.fetch_add(1, std::memory_order_relaxed)

#else

#define AIKO_DECLARE_COPY_MOVE_COUNTERS()
#define AIKO_COUNT_COPY_CTOR()
#define AIKO_COUNT_MOVE_CTOR()
#define AIKO_COUNT_COPY_ASSIGN()
#define AIKO_COUNT_MOVE_ASSIGN()
#endif