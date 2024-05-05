#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <tuple>

#include "nes_types.h"
#include "microprocessor.h"

namespace test { class MemoryTest; }

namespace nes
{

    class Memory : public Microprocessor
    {
        friend class Cartridge;
        friend class test::MemoryTest;
    public:
        static constexpr u32 MAX_MEM = 1024 * 64; // 64KB

        static constexpr std::tuple<Word, Word>STACK_PAGE = { 0x0100, 0x01FF };

        virtual void reset() override;

        Byte read(Byte);
        void write(Byte, Byte);

        Byte read(Word);
        void write(Word, Byte);
        void write(Word, Word);

    private:
        Byte data[MAX_MEM];
    };

}
