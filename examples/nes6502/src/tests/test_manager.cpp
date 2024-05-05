#include "test_manager.h"

#include <memory.h>
#include <assert.h>

#include "test.h"
#include "test_type.h"
#include "test_cpu.h"
#include "test_core.h"
#include "test_memory.h"

#include "online/online_test_manager.h"

namespace test
{
    void TestManager::init()
    {
        tests.emplace_back(std::make_unique<CoreTest>());
        tests.emplace_back(std::make_unique<MemoryTest>());
        tests.emplace_back(std::make_unique<CPUTest>());
    }

    void TestManager::runAll()
    {
        TestResult result;
        for (auto& t : tests)
        {
            if (t->run().success == true)
            {
                aiko::Log::info("Passed test: ", t->name() );
            }
            else
            {

                aiko::Log::error("Test failed: ", t->name());
                result.success = false;
            }
        }
        if (result.success == false)
        {
            aiko::Log::error("Not all test passed. Please fix above fails");
        }
        assert(result.success, "Tests not passed");


        online::TestManager tests;
        tests.run();

    }

}
