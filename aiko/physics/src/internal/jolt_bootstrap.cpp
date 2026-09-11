#include "jolt_bootstrap.h"

#include <mutex>

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>

namespace aiko::physics
{
    namespace
    {
        std::mutex g_joltMutex;
        int g_joltRefCount = 0;
    }

    void acquireJolt()
    {
        std::scoped_lock lock(g_joltMutex);

        if (g_joltRefCount == 0)
        {
            JPH::RegisterDefaultAllocator();

            JPH::Factory::sInstance = new JPH::Factory();
            JPH::RegisterTypes();
        }

        ++g_joltRefCount;
    }

    void releaseJolt()
    {
        std::scoped_lock lock(g_joltMutex);

        if (g_joltRefCount == 0)
        {
            return;
        }

        --g_joltRefCount;

        if (g_joltRefCount == 0)
        {
            JPH::UnregisterTypes();
            delete JPH::Factory::sInstance;
            JPH::Factory::sInstance = nullptr;
        }
    }
}