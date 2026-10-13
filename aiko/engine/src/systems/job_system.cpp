#include "job_system.h"

#include <chrono>
#include <thread>
#include <utility>

namespace aiko
{

    namespace
    {
        constexpr float MainThreadJobBudgetMs = 2.0f;
    }

    JobSystem::~JobSystem()
    {
        shutdown();
    }

    void JobSystem::init()
    {
        m_executor.start(resolveWorkerCount());
        m_acceptingJobs.store(true, std::memory_order_release);
    }

    void JobSystem::update()
    {
        processMainThreadJobs(MainThreadJobBudgetMs);
    }

    void JobSystem::dispose()
    {
        shutdown();
    }

    void JobSystem::submit(BackgroundJob job)
    {
        if (m_acceptingJobs.load(std::memory_order_acquire) == false)
        {
            return;
        }
        m_executor.submit(std::move(job));
    }

    void JobSystem::dispatchMainThread(MainThreadJob job)
    {
        if (job.valid() == false)
        {
            return;
        }

        if (m_acceptingJobs.load(std::memory_order_acquire) == false)
        {
            return;
        }

        std::scoped_lock lock(m_mainThreadMutex);

        if (m_acceptingJobs.load(std::memory_order_relaxed) == false)
        {
            return;
        }

        m_mainThreadJobs.emplace_back(std::move(job));
    }

    JobStats JobSystem::stats() const
    {
        const JobExecutorStats background = m_executor.stats();

        JobStats result;
        result.workerThreads = background.workerThreads;
        result.queuedJobs = background.queuedJobs;
        result.activeJobs = background.activeJobs;

        {
            std::scoped_lock lock(m_mainThreadMutex);
            result.pendingMainThreadJobs = m_mainThreadJobs.size();
        }

        return result;
    }

    void JobSystem::processMainThreadJobs(float budgetMs)
    {
        using Clock = std::chrono::steady_clock;

        const auto start = Clock::now();

        while (true)
        {
            MainThreadJob job;

            {
                std::scoped_lock lock(m_mainThreadMutex);

                if (m_mainThreadJobs.empty())
                {
                    return;
                }

                job = std::move(m_mainThreadJobs.front());
                m_mainThreadJobs.pop_front();
            }

            job();

            if (budgetMs <= 0.0f)
            {
                continue;
            }

            const auto elapsed = std::chrono::duration<float, std::milli>(Clock::now() - start);

            if (elapsed.count() >= budgetMs)
            {
                return;
            }
        }
    }

    void JobSystem::shutdown()
    {
        {
            std::scoped_lock lock(m_mainThreadMutex);
            m_acceptingJobs.store(false, std::memory_order_release);
        }

        // Do not hold the completion mutex while joining workers:
        // a running job may still attempt to dispatch a completion.
        m_executor.shutdown();

        std::deque<MainThreadJob> cancelledJobs;

        {
            std::scoped_lock lock(m_mainThreadMutex);
            cancelledJobs.swap(m_mainThreadJobs);
        }

        // Destroy completion captures outside the mutex.
        cancelledJobs.clear();
    }

    uint32_t JobSystem::resolveWorkerCount() const
    {
        const uint32_t hardware = std::thread::hardware_concurrency();

        if (hardware <= 2)
        {
            return 1;
        }

        return hardware - 2;
    }
}
