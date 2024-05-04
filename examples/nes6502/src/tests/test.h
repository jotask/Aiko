#pragma once

#include <aiko_types.h>
#include <vector>
#include <assert.h>

#include "aiko_types.h"
#include "test_type.h"

namespace test
{

    class Test
    {
    public:
        virtual const char* name() = 0;
        virtual TestResult run() = 0;
    };

}
