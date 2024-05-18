#pragma once

#include <aiko_types.h>
#include <vector>
#include <assert.h>
#include <exception>
#include <assert.h>

#include "aiko_types.h"

#define ASSERT_TESTS

namespace test
{

    #define RUN_TEST( fnt ) if (fnt().success == false) result.success = false;

#ifdef ASSERT_TESTS
    #define TEST_TRUE(condition, message ) if (!(condition)) { aiko::Log::error(message); result.success = false; assert(false, message); };
#else
    #define TEST_TRUE(condition, message ) if (!(condition)) { aiko::Log::error(message); result.success = false; };
#endif

    struct TestResult
    {
        bool success = true;
    };

}
