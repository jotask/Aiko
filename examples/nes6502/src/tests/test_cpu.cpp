#include "test_cpu.h"

#include <assert.h>

#include "bus.h"
#include "cpu.h"
#include "memory.h"
#include "cartridge.h"
#include "test_type.h"

namespace test
{

    TestResult CPUTest::run()
    {
        TestResult result;
        RUN_TEST(test_lda_inmediate);
        RUN_TEST(test_lda_absolute);
        return result;
    }

    static void insertBytes(std::vector<nes::Byte>& buffer, std::vector<nes::Byte> insert)
    {
        // buffer.reserve(buffer.size() + insert.size());
        buffer.insert(buffer.end(), insert.begin(), insert.end());
    }

    TestResult CPUTest::test_lda_inmediate()
    {
        TestResult result;
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Cartridge* cart = bus->getMicroprocesor<nes::Cartridge>();
        std::vector<nes::Byte> dump;
        insertBytes(dump, { 0xa9 , 0x13 }); // lda inmediate
        cart->load(dump);

        for (std::size_t i = 0 ; i < dump.size(); i += 2)
        {
            bus->clock();
        }

        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        assert(cpu != nullptr, "Couldnt' get cpu from bus");

        TEST_TRUE(cpu->A == 0x13, "lda inmediate not working");

        return result;
    }

    TestResult CPUTest::test_lda_zeroPage()
    {
        return TestResult();
    }

    TestResult CPUTest::test_lda_zeroPageX()
    {
        return TestResult();
    }

    TestResult CPUTest::test_lda_absolute()
    {
        TestResult result;
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Cartridge* cart = bus->getMicroprocesor<nes::Cartridge>();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        nes::Memory* mem = bus->getMicroprocesor<nes::Memory>();
        // mem->write(0x13FF, 0xAA );
        std::vector<nes::Byte> dump;
        insertBytes(dump, { 0xad , 0x13, 0xFF }); // lda absolute
        cart->load(dump);

        for (std::size_t i = 0; i < dump.size(); i += 3)
        {
            bus->clock();
        }

        assert(cpu != nullptr, "Couldnt' get cpu from bus");

        TEST_TRUE(cpu->A == 0x13, "lda absolute not working");

        return result;
    }

    TestResult CPUTest::test_lda_absoluteX()
    {
        return TestResult();
    }

    TestResult CPUTest::test_lda_absoluteY()
    {
        return TestResult();
    }

    TestResult CPUTest::test_lda_indirectX()
    {
        return TestResult();
    }

    TestResult CPUTest::test_lda_indirectY()
    {
        return TestResult();
    }

}
