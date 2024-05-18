#pragma once

#include <vector>
#include "online_test.h"

namespace test::online
{
    class TestManager
    {
    public:
        TestManager();
        void run();
    private:
        std::vector<OnlinesTest> tests;
    };
}
