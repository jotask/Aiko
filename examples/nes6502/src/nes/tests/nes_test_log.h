#pragma once

#include <aiko_includes.h>
#include <string>
#include <vector>
#include "nes/cpu/instructions.h"

#include "nes/nes_types.h"

namespace nes
{
    class Cpu;
    class Memory;
    namespace test
    {

        class NesTest : public aiko::Singleton<NesTest>
        {
        private:
            struct LineCode
            {
                Word pc;
                Byte one;
                Byte two;
                Byte thr;
                aiko::string instruction;

                Byte a;
                Byte x;
                Byte y;
                Byte p;
                Byte sp;

                Byte ppu_one;
                Byte ppu_two;

                Word cycle;

            };

            void init();

            std::vector<LineCode> lines;
            bool initialized;

        public:

            void test(Word line, OpCode ops, Word programCounter, Byte stackPointer, Byte A, Byte X, Byte Y, Byte P);

        };

    }
}
