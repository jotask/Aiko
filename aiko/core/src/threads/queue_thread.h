#pragma once

#include "thread_worker.h"

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "job_types.h"

namespace aiko
{

    class QueueThread : public ThreadWorker
    {

    public:

        using Job = std::function<JobResponse()>;

        QueueThread();
        virtual ~QueueThread() override;

        void enqueue(Job job);

    protected:

        virtual void run() override;
        virtual void onStop() override;

    private:

        std::mutex queueMutex;
        std::condition_variable condition;
        std::queue<Job> jobs;

    };

}
