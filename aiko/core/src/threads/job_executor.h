#pragma once

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <future>
#include <mutex>
#include <thread>
#include <vector>

namespace aiko
{
    struct JobExecutorStats
    {
        uint32_t workerThreads = 0;
        std::size_t queuedJobs = 0;
        std::size_t activeJobs = 0;
    };

    class JobExecutor final
    {
    public:
        using Job = std::packaged_task<void()>;

        JobExecutor() = default;
        ~JobExecutor();

        JobExecutor(const JobExecutor&) = delete;
        JobExecutor& operator=(const JobExecutor&) = delete;
        JobExecutor(JobExecutor&&) = delete;
        JobExecutor& operator=(JobExecutor&&) = delete;

        // Lifecycle calls must be serialized by the owner.
        // Never call them from this executor's workers.
        void start(uint32_t workerCount);
        void shutdown();

        // Thread-safe while the executor remains alive.
        // Accepted jobs may be cancelled by shutdown before execution.
        bool submit(Job job);
        JobExecutorStats stats() const;

    private:
        void workerLoop();

        mutable std::mutex m_mutex;
        std::condition_variable m_condition;
        std::deque<Job> m_jobs;
        std::vector<std::jthread> m_workers;

        uint32_t m_workerCount = 0;
        std::size_t m_activeJobs = 0;
        bool m_acceptingJobs = false;
    };
}
