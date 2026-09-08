#pragma once

#include <cstddef>
#include <cstdint>

namespace aiko
{
    struct JobStats
    {
        uint32_t workerThreads = 0;
        std::size_t queuedJobs = 0;
        std::size_t activeJobs = 0;
        std::size_t pendingMainThreadJobs = 0;
    };
}
