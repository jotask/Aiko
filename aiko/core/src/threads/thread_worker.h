#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <atomic>
#include <iostream>

namespace aiko
{

    class ThreadWorker
    {
    public:
        ThreadWorker();
        virtual ~ThreadWorker();

        void start();
        void stop();
        void join();
        bool isRunning() const;

    protected:

        virtual void run() = 0;
        virtual void onStop() {}
        std::atomic<bool> running;

    private:

        std::thread worker;
        void entryPoint();

    };

}
