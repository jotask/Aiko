#include "thread_worker.h"

namespace aiko
{

    ThreadWorker::ThreadWorker()
        : running(false)
    {

    }

    ThreadWorker::~ThreadWorker()
    {
        stop();
        join();
    }

    void ThreadWorker::start()
    {
        running = true;
        worker = std::thread(&ThreadWorker::entryPoint, this);
    }

    void ThreadWorker::stop()
    {
        running = false;
        onStop();
    }

    void ThreadWorker::join()
    {
        if (worker.joinable() == true)
        {
            worker.join();
        }
    }

    bool ThreadWorker::isRunning() const
    {
        return running;
    }

    void ThreadWorker::entryPoint()
    {
        run();
    }
}
