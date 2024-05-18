#pragma once

#include <string>

#include "online_test_types.h"

namespace test::online
{

    class OnlinesTest
    {
    public:
        OnlinesTest(std::string test);
        std::string name;

        CpuState initial;
        CpuState final;
        CycleState cycles;

        void run();

    };

}
