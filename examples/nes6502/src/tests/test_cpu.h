#pragma once

#include <aiko_types.h>
#include <vector>
#include <exception>

#include "nes6502.h"

#include "test.h"

namespace test
{

    class CPUTest : public Test
    {
    public:
        virtual const char* name() { return "CpuTest"; };
        virtual TestResult run() override;

        // Addresing Modes
        TestResult test_lda_inmediate();
        TestResult test_lda_zeroPage();
        TestResult test_lda_zeroPageX();
        TestResult test_lda_absolute();
        TestResult test_lda_absoluteX();
        TestResult test_lda_absoluteY();
        TestResult test_lda_indirectX();
        TestResult test_lda_indirectY();

    private:
        void set_memory_address_zero_page(nes::nes6502*, nes::Byte, nes::Byte);
        void set_memory_address_zero_page_x(nes::nes6502*, nes::Byte, nes::Byte);
        void set_memory_address_absolute(nes::nes6502*, nes::Word, nes::Byte);
        void set_memory_address_absolute_x(nes::nes6502*, nes::Word, nes::Byte);
        void set_memory_address_absolute_y(nes::nes6502*, nes::Word, nes::Byte);
        void set_memory_address_indirect_x(nes::nes6502*, nes::Word, nes::Byte);
        void set_memory_address_indirect_y(nes::nes6502*, nes::Word, nes::Byte);
    };

}