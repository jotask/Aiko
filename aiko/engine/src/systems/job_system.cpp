#include "job_system.h"

#include <chrono>

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
        const uint32_t workerCount = resolveWorkerCount();

        m_acceptingJobs.store(true, std::memory_order_release);

        m_workers.reserve(workerCount);

        for (uint32_t i = 0; i < workerCount; ++i)
        {
            m_workers.emplace_back(
                [this](std::stop_token stopToken)
                {
                    workerLoop(stopToken);
                }
            );
        }
    }

    void JobSystem::update()
    {
        processMainThreadJobs(MainThreadJobBudgetMs);
    }

    void JobSystem::dispose()
    {
        shutdown();

        std::scoped_lock lock(m_mainThreadMutex);
        m_mainThreadJobs.clear();
    }

    void JobSystem::submit(BackgroundJob job)
    {
        if (m_acceptingJobs.load(std::memory_order_acquire) == false)
        {
            return;
        }

        {
            std::scoped_lock lock(m_jobMutex);

            if (m_acceptingJobs.load(std::memory_order_relaxed) == false)
            {
                return;
            }

            m_jobs.emplace_back(std::move(job));
        }

        m_jobCondition.notify_one();
    }

    void JobSystem::dispatchMainThread(MainThreadJob job)
    {
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
        JobStats result;
        result.workerThreads = static_cast<uint32_t>(m_workers.size());
        result.activeJobs = m_activeJobs.load(std::memory_order_relaxed);

        {
            std::scoped_lock lock(m_jobMutex);
            result.queuedJobs = m_jobs.size();
        }

        {
            std::scoped_lock lock(m_mainThreadMutex);
            result.pendingMainThreadJobs = m_mainThreadJobs.size();
        }

        return result;
    }

    void JobSystem::workerLoop(std::stop_token stopToken)
    {
        while (stopToken.stop_requested() == false)
        {
            BackgroundJob job;

            {
                std::unique_lock lock(m_jobMutex);

                m_jobCondition.wait(
                    lock,
                    stopToken,
                    [this]
                    {
                        return m_jobs.empty() == false || m_acceptingJobs.load(std::memory_order_acquire) == false;
                    }
                );

                if (stopToken.stop_requested())
                {
                    return;
                }

                if (m_jobs.empty())
                {
                    continue;
                }

                job = std::move(m_jobs.front());
                m_jobs.pop_front();
            }

            m_activeJobs.fetch_add(1, std::memory_order_relaxed);

            job();

            m_activeJobs.fetch_sub(1, std::memory_order_relaxed);
        }
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
        if (m_workers.empty())
        {
            return;
        }

        m_acceptingJobs.store(false, std::memory_order_release);

        {
            std::scoped_lock lock(m_jobMutex);
            m_jobs.clear();
        }

        for (std::jthread& worker : m_workers)
        {
            worker.request_stop();
        }

        m_jobCondition.notify_all();

        m_workers.clear();
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
