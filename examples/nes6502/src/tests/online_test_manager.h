#pragma once

#include <vector>
#include <aiko_includes.h>
#include "online_test.h"

namespace nes::test::online
{
    class TestManager : public aiko::Singleton<TestManager>
    {
    public:
        void init();
        void run();
    private:
        std::vector<OnlinesTest> tests;
    };
}
