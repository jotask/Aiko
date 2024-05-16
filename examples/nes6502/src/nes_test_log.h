#pragma once

#include <aiko_includes.h>
#include <string>
#include <vector>

#include "nes_types.h"

namespace nes::test
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
            std::string instruction;

            Byte a;
            Byte x;
            Byte y;
            Byte p;
            Byte sp;
        };

        void init();

        std::vector<LineCode> lines;
        bool initialized;

    public:

        void test(Word line, Word programCounter, Byte stackPointer, Byte A, Byte X, Byte Y, Byte P);

    };

}
