#include "test_memory.h"

#include <assert.h>

#include "bus.h"
#include "cpu.h"
#include "memory.h"
#include "cartridge.h"
#include "test_type.h"
#include "nes_utils.h"

namespace test
{

    TestResult MemoryTest::run()
    {
        TestResult result;
        RUN_TEST(test_write);
        RUN_TEST(test_read);
        return result;
    }

    TestResult MemoryTest::test_write()
    {
        TestResult result;
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Memory* mem = bus->getMicroprocesor<nes::Memory>();
        assert(mem != nullptr, "Couldnt' get Memory from bus");
        nes::Byte memoryAddress = 0x13;
        nes::Byte memoryValue = 0x11;
        // Write Byte -> byte
        mem->write(memoryAddress, memoryValue);
        TEST_TRUE(mem->data[memoryAddress] == memoryValue, "lda inmediate not working");
        nes::Word memoryAddress2 = 0xFF13;
        // Write Word -> byte
        mem->write(memoryAddress2, memoryValue);
        TEST_TRUE(mem->data[memoryAddress2] == memoryValue, "lda inmediate not working");
        nes::Word memoryValue2 = 0x1122;
        // Write Word -> Word
        mem->write(memoryAddress2, memoryValue2);
        TEST_TRUE(mem->data[memoryAddress2 + 0] == nes::getHigh(memoryValue2) , "lda inmediate not working");
        TEST_TRUE(mem->data[memoryAddress2 + 1] == nes::getLow (memoryValue2) , "lda inmediate not working");
        return result;
    }

    TestResult MemoryTest::test_read()
    {
        TestResult result;
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Memory* mem = bus->getMicroprocesor<nes::Memory>();
        assert(mem != nullptr, "Couldnt' get Memory from bus");
        nes::Byte memoryValue = 0x11;
        // Read Byte -> byte
        {
            nes::Byte memoryAddress = 0x13;
            mem->data[memoryAddress] = memoryValue;
            nes::Byte res = mem->read(memoryAddress);
            TEST_TRUE(res == memoryValue, "lda inmediate not working");
        }
        // Read Word -> byte
        {
            nes::Word memoryAddress = 0x13FF;
            mem->data[memoryAddress] = memoryValue;
            nes::Byte res = mem->read(memoryAddress);
            TEST_TRUE(res == memoryValue, "lda inmediate not working");
        }
        // Read Word -> Word
        {
            nes::Word memoryAddress = 0x13FF;
            nes::Word memoryValueWord = 0x1133;
            nes::Byte high = nes::getHigh(memoryValueWord);
            nes::Byte low = nes::getLow(memoryValueWord);

            mem->data[memoryAddress + 0] = high;
            mem->data[memoryAddress + 1] = low;

            nes::Byte res1 = mem->read(static_cast<nes::Word>(memoryAddress + 0));
            nes::Byte res2 = mem->read(static_cast<nes::Word>(memoryAddress + 1));

            TEST_TRUE(res1 == high, "lda inmediate not working");
            TEST_TRUE(res2 == low, "lda inmediate not working");

        }
        
        return result;
    }

}
