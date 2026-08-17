#include "queue_thread.h"

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace aiko
{

    QueueThread::QueueThread() = default;

    void QueueThread::enqueue(Job job)
    {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            jobs.push(std::move(job));
        }
        condition.notify_one();
    }

    void QueueThread::run()
    {
        while (running == true)
        {
            Job job = nullptr;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [&]()
                {
                    return jobs.empty() == false || running == false;
                });

                if (running == false && jobs.empty() == true)
                {
                    break;
                }

                job = std::move(jobs.front());
                jobs.pop();
            }
            if (job != nullptr)
            {
                job();
            }

        }
    }

    void QueueThread::onStop()
    {
        ThreadWorker::onStop();
        condition.notify_all();
    }
}
