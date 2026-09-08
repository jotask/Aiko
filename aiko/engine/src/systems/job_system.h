#pragma once

#include "systems/base_system.h"

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <future>
#include <cstdint>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

#include "types/job_types.h"

namespace aiko
{

    class JobSystem final : public BaseSystem
    {
    public:
        using BackgroundJob = std::packaged_task<void()>;
        using MainThreadJob = std::packaged_task<void()>;

        JobSystem() = default;
        ~JobSystem() override;

        void submit(BackgroundJob job);
        void dispatchMainThread(MainThreadJob job);

        JobStats stats() const;

    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void dispose() override;

    private:
        void workerLoop(std::stop_token stopToken);

        void processMainThreadJobs(float budgetMs);
        void shutdown();

        uint32_t resolveWorkerCount() const;

    private:
        mutable std::mutex m_jobMutex;
        std::condition_variable_any m_jobCondition;
        std::deque<BackgroundJob> m_jobs;

        mutable std::mutex m_mainThreadMutex;
        std::deque<MainThreadJob> m_mainThreadJobs;

        std::vector<std::jthread> m_workers;

        std::atomic_size_t m_activeJobs{0};
        std::atomic_bool m_acceptingJobs{false};
    };
}
