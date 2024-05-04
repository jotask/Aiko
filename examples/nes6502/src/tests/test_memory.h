#pragma once

#include <aiko_types.h>
#include <vector>
#include <exception>

#include "nes6502.h"

#include "test.h"

namespace test
{

    class MemoryTest : public Test
    {
    public:
        virtual const char* name() { return "MemoryTest"; };
        virtual TestResult run() override;

        TestResult test_write();
        TestResult test_read();

    };

}