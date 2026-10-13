#pragma once

#include "systems/base_system.h"
#include "types/job_types.h"

#include <threads/job_executor.h>

#include <atomic>
#include <cstdint>
#include <deque>
#include <future>
#include <mutex>

namespace aiko
{
    class JobSystem final : public BaseSystem
    {
    public:
        using BackgroundJob = JobExecutor::Job;
        using MainThreadJob = std::packaged_task<void()>;

        JobSystem() = default;
        ~JobSystem() override;

        void submit(BackgroundJob job);
        void dispatchMainThread(MainThreadJob job);

        JobStats stats() const;

    protected:
        SystemUpdatePhase updatePhase() const override
        {
            return SystemUpdatePhase::EarlyUpdate;
        }

        void init() override;
        void update() override;
        void dispose() override;

        bool updateWhenPaused() const override { return true; }

    private:
        void processMainThreadJobs(float budgetMs);
        void shutdown();
        uint32_t resolveWorkerCount() const;

        mutable std::mutex m_mainThreadMutex;
        std::deque<MainThreadJob> m_mainThreadJobs;
        std::atomic_bool m_acceptingJobs{false};

        JobExecutor m_executor;
    };
}
