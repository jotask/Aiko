#pragma once

#include <aiko_types.h>
#include <vector>
#include <exception>

#include "nes6502.h"

#include "test.h"

namespace test
{

    class CoreTest : public Test
    {
    public:
        virtual const char* name() { return "CoreTest"; };
        virtual TestResult run() override;

        // utils
        TestResult test_toLow();
        TestResult test_toHigh();
        TestResult test_toWord();

    };

}