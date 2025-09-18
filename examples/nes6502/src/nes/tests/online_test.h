#pragma once

#include <aiko_types.h>

#include "online_test_types.h"

namespace nes::test::online
{

    class OnlinesTest
    {
    public:
        OnlinesTest(aiko::string test);
        aiko::string name;

        CpuState initial;
        CpuState final;
        CycleState cycles;

        void run();

    };

}
