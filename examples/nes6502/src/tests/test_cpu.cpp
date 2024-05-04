#include "test_cpu.h"

#include <assert.h>

#include "bus.h"
#include "cpu.h"
#include "memory.h"
#include "cartridge.h"
#include "test_type.h"
#include "nes_utils.h"

namespace test
{

    TestResult CPUTest::run()
    {
        TestResult result;
        RUN_TEST(test_lda_inmediate);
        RUN_TEST(test_lda_absolute);
        RUN_TEST(test_lda_zeroPage);
        RUN_TEST(test_lda_zeroPageX);
        RUN_TEST(test_lda_absoluteX);
        RUN_TEST(test_lda_absoluteY);
        RUN_TEST(test_lda_indirectX);
        RUN_TEST(test_lda_indirectY);
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
        TestResult result;
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Cartridge* cart = bus->getMicroprocesor<nes::Cartridge>();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        assert(cpu != nullptr, "Couldnt' get cpu from bus");
        std::vector<nes::Byte> dump;

        constexpr nes::Byte memoryAddress = 0x13;
        constexpr nes::Byte memoryValue = 0xaa;
        insertBytes(dump, { 0xa5 , memoryAddress }); // lda zeroPage
        cart->load(dump);

        set_memory_address_zero_page(&nes, memoryAddress, memoryValue);

        for (std::size_t i = 0; i < dump.size(); i += 2)
        {
            bus->clock();
        }

        TEST_TRUE(cpu->A == memoryValue, "lda zeroPage not working");

        return result;
    }

    TestResult CPUTest::test_lda_zeroPageX()
    {
        TestResult result;
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Cartridge* cart = bus->getMicroprocesor<nes::Cartridge>();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        assert(cpu != nullptr, "Couldnt' get cpu from bus");
        constexpr nes::Byte memoryAddress = 0x13;
        constexpr nes::Byte memoryValue = 0xaa;
        std::vector<nes::Byte> dump;
        insertBytes(dump, { 0xb5 , memoryAddress }); // lda zeroPageX
        cart->load(dump);

        cpu->X = 1;
        set_memory_address_zero_page_x(&nes, memoryAddress, memoryValue);

        for (std::size_t i = 0; i < dump.size(); i += 2)
        {
            bus->clock();
        }

        TEST_TRUE(cpu->A == memoryValue, "lda zeroPage x not working");

        return result;
    }

    TestResult CPUTest::test_lda_absolute()
    {
        TestResult result;
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Cartridge* cart = bus->getMicroprocesor<nes::Cartridge>();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        assert(cpu != nullptr, "Couldnt' get cpu from bus");
        constexpr nes::Word memoryAddress = 0x013ff;
        constexpr nes::Byte memoryValue = 0xaa;
        std::vector<nes::Byte> dump;
        insertBytes(dump, { 0xad , nes::getHigh(memoryAddress) , nes::getLow(memoryAddress)}); // lda absolute
        cart->load(dump);

        set_memory_address_absolute(&nes, memoryAddress, memoryValue);

        for (std::size_t i = 0; i < dump.size(); i += 4)
        {
            bus->clock();
        }

        TEST_TRUE(cpu->A == memoryValue, "lda absolute not working");

        return result;
    }

    TestResult CPUTest::test_lda_absoluteX()
    {
        TestResult result;
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Cartridge* cart = bus->getMicroprocesor<nes::Cartridge>();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        assert(cpu != nullptr, "Couldnt' get cpu from bus");
        constexpr nes::Word memoryAddress = 0x13ff;
        constexpr nes::Byte memoryValue = 0xaa;
        std::vector<nes::Byte> dump;
        insertBytes(dump, { 0xbd , nes::getHigh(memoryAddress), nes::getLow(memoryAddress)}); // lda zeroPageX
        cart->load(dump);

        cpu->X = 1;
        set_memory_address_absolute_x(&nes, memoryAddress, memoryValue);

        for (std::size_t i = 0; i < dump.size(); i += 2)
        {
            bus->clock();
        }

        TEST_TRUE(cpu->A == memoryValue, "lda absolute X not working");

        return result;
    }

    TestResult CPUTest::test_lda_absoluteY()
    {
        TestResult result;
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Cartridge* cart = bus->getMicroprocesor<nes::Cartridge>();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        assert(cpu != nullptr, "Couldnt' get cpu from bus");
        constexpr nes::Word memoryAddress = 0x13ff;
        constexpr nes::Byte memoryValue = 0xaa;
        std::vector<nes::Byte> dump;
        insertBytes(dump, { 0xb9 , nes::getHigh(memoryAddress), nes::getLow(memoryAddress) }); // lda zeroPageX
        cart->load(dump);

        cpu->Y = 1;
        set_memory_address_absolute_y(&nes, memoryAddress, memoryValue);

        for (std::size_t i = 0; i < dump.size(); i += 2)
        {
            bus->clock();
        }

        TEST_TRUE(cpu->A == memoryValue, "lda absolute Y not working");

        return result;
    }

    TestResult CPUTest::test_lda_indirectX()
    {
        TestResult result;
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Cartridge* cart = bus->getMicroprocesor<nes::Cartridge>();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        assert(cpu != nullptr, "Couldnt' get cpu from bus");
        nes::Byte memoryAddress = 0x13;
        nes::Byte memoryValue = 0xaa;

        std::vector<nes::Byte> dump;
        insertBytes(dump, { 0xa1 , memoryAddress }); // lda zeroPageX
        cart->load(dump);

        cpu->X = 1;
        set_memory_address_indirect_x(&nes, memoryAddress, memoryValue);

        for (std::size_t i = 0; i < dump.size(); i += 2)
        {
            bus->clock();
        }

        TEST_TRUE(cpu->A == memoryValue, "lda indirect X not working");

        return result;
    }

    TestResult CPUTest::test_lda_indirectY()
    {
        TestResult result;
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Cartridge* cart = bus->getMicroprocesor<nes::Cartridge>();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        assert(cpu != nullptr, "Couldnt' get cpu from bus");
        constexpr nes::Byte memoryAddress = 0x13;
        constexpr nes::Byte memoryValue = 0xaa;
        std::vector<nes::Byte> dump;
        insertBytes(dump, { 0xb1 , memoryAddress }); // lda zeroPageX
        cart->load(dump);
        cpu->Y = 1;
        set_memory_address_indirect_y(&nes, memoryAddress, memoryValue);

        for (std::size_t i = 0; i < dump.size(); i += 2)
        {
            bus->clock();
        }

        TEST_TRUE(cpu->A == memoryValue, "lda indirect X not working");

        return result;
    }

}
