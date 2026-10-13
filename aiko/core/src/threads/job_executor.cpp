#include "job_executor.h"

#include "aiko_macros.h"

#include <utility>

namespace aiko
{
    JobExecutor::~JobExecutor()
    {
        shutdown();
    }

    void JobExecutor::start(uint32_t workerCount)
    {
        AIKO_ASSERT(workerCount > 0, "JobExecutor requires at least one worker");
        AIKO_ASSERT(m_workers.empty(), "JobExecutor is already started");

        try
        {
            std::unique_lock lock(m_mutex);

            m_workers.reserve(workerCount);

            for (uint32_t i = 0; i < workerCount; ++i)
            {
                m_workers.emplace_back([this] { workerLoop();});
            }

            m_workerCount = workerCount;
            m_acceptingJobs = true;
        }
        catch (...)
        {
            shutdown();
            throw;
        }
    }

    bool JobExecutor::submit(Job job)
    {
        if (job.valid() == false)
        {
            return false;
        }

        {
            std::scoped_lock lock(m_mutex);

            if (m_acceptingJobs == false)
            {
                return false;
            }

            m_jobs.emplace_back(std::move(job));
        }

        m_condition.notify_one();
        return true;
    }

    JobExecutorStats JobExecutor::stats() const
    {
        std::scoped_lock lock(m_mutex);

        return JobExecutorStats
        {
            .workerThreads = m_workerCount,
            .queuedJobs = m_jobs.size(),
            .activeJobs = m_activeJobs,
        };
    }

    void JobExecutor::workerLoop()
    {
        while (true)
        {
            Job job;

            {
                std::unique_lock lock(m_mutex);

                m_condition.wait(
                    lock,
                    [this]
                    {
                        return !m_acceptingJobs || !m_jobs.empty();
                    });

                if (m_acceptingJobs == false)
                {
                    return;
                }

                job = std::move(m_jobs.front());
                m_jobs.pop_front();
                ++m_activeJobs;
            }

            job();

            {
                std::scoped_lock lock(m_mutex);
                --m_activeJobs;
            }
        }
    }

    void JobExecutor::shutdown()
    {
        for (const std::jthread& worker : m_workers)
        {
            AIKO_ASSERT(worker.get_id() != std::this_thread::get_id(), "JobExecutor cannot shut down from its own worker");
        }

        std::deque<Job> cancelledJobs;

        {
            std::scoped_lock lock(m_mutex);

            m_acceptingJobs = false;
            cancelledJobs.swap(m_jobs);
        }

        m_condition.notify_all();

        cancelledJobs.clear();

        m_workers.clear();

        {
            std::scoped_lock lock(m_mutex);
            m_workerCount = 0;
        }
    }
}
