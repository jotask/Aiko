#pragma once

#include <aiko_types.h>
#include <vector>

namespace test
{

    class Test;
    class TestManager
    {
    public:
        void init();
        void runAll();
    public:
        std::vector<aiko::AikoPtr<Test>> tests;
    };

}