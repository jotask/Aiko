#include "test_cpu.h"

#include "bus.h"
#include "memory.h"
#include "cartridge.h"
#include "test_core.h"
#include "nes_utils.h"
#include "test_type.h"

namespace test
{

    TestResult CoreTest::run()
    {
        TestResult result;
        RUN_TEST(test_toLow);
        RUN_TEST(test_toHigh);
        RUN_TEST(test_toWord);
        return result;
    }

    TestResult CoreTest::test_toLow()
    {
        TestResult result;
        nes::Word test = 0x13FF;
        nes::Byte low = nes::getLow(test);
        TEST_TRUE(low == 0xFF, "Low test not passed");
        return result;
    }

    TestResult CoreTest::test_toHigh()
    {
        TestResult result;
        nes::Word test = 0x13FF;
        nes::Byte high = nes::getHigh(test);
        TEST_TRUE(high == high, "High test not passed");
        return result;
    }

    TestResult CoreTest::test_toWord()
    {
        TestResult result;
        nes::Byte low = 0xFF;
        nes::Byte high = 0x13;
        nes::Word word = nes::toWord(high, low);
        TEST_TRUE(word == 0x13FF, "To word test not passed");
        return result;

    }

}
