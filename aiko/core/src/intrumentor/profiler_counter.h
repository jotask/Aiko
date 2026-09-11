#pragma once

#include <atomic>
#include <cstdint>
#include <utility>

namespace aiko::profile
{
    struct CopyMoveCounters
    {
        std::atomic<uint64_t> copyCtorCount{0};
        std::atomic<uint64_t> moveCtorCount{0};
        std::atomic<uint64_t> copyAssignCount{0};
        std::atomic<uint64_t> moveAssignCount{0};

        void reset()
        {
            copyCtorCount.store(0, std::memory_order_relaxed);
            moveCtorCount.store(0, std::memory_order_relaxed);
            copyAssignCount.store(0, std::memory_order_relaxed);
            moveAssignCount.store(0, std::memory_order_relaxed);
        }
    };

    template<typename T>
    struct CopyMoveTracked
    {
#if defined(AIKO_PROFILE_COPIES)
        inline static CopyMoveCounters s_counters{};
#endif

        CopyMoveTracked() = default;

        CopyMoveTracked(const CopyMoveTracked&)
        {
#if defined(AIKO_PROFILE_COPIES)
            s_counters.copyCtorCount.fetch_add(1, std::memory_order_relaxed);
#endif
        }

        CopyMoveTracked(CopyMoveTracked&&) noexcept
        {
#if defined(AIKO_PROFILE_COPIES)
            s_counters.moveCtorCount.fetch_add(1, std::memory_order_relaxed);
#endif
        }

        CopyMoveTracked& operator=(const CopyMoveTracked&)
        {
#if defined(AIKO_PROFILE_COPIES)
            s_counters.copyAssignCount.fetch_add(1, std::memory_order_relaxed);
#endif
            return *this;
        }

        CopyMoveTracked& operator=(CopyMoveTracked&&) noexcept
        {
#if defined(AIKO_PROFILE_COPIES)
            s_counters.moveAssignCount.fetch_add(1, std::memory_order_relaxed);
#endif
            return *this;
        }

#if defined(AIKO_PROFILE_COPIES)
        static void resetCounters()
        {
            s_counters.reset();
        }
#endif
    };

    #if defined(AIKO_PROFILE_COPIES)
        #define AIKO_PRINT_PROFILER_COUNTERS(classType)                                                         \
        do                                                                                                      \
        {                                                                                                       \
            static float accum = 0.0f;                                                                          \
            accum += aiko::Time::it().getDeltaTime();                                                           \
            if (accum >= 1.0f)                                                                                  \
            {                                                                                                   \
                accum = 0.0f;                                                                                   \
                logger::Log::info(                                                                              \
                #classType " copies: ctor=%llu moveCtor=%llu copyAssign=%llu moveAssign=%llu",                  \
                (unsigned long long)classType::s_counters.copyCtorCount.load(std::memory_order_relaxed),        \
                (unsigned long long)classType::s_counters.moveCtorCount.load(std::memory_order_relaxed),        \
                (unsigned long long)classType::s_counters.copyAssignCount.load(std::memory_order_relaxed),      \
                (unsigned long long)classType::s_counters.moveAssignCount.load(std::memory_order_relaxed)       \
                );                                                                                              \
                classType::resetCounters();                                                                     \
            }                                                                                                   \
        } while (0)
    #else
        #define AIKO_PRINT_PROFILER_COUNTERS(classType)
    #endif

}