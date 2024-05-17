#include "nes_test_log.h"

#include <fstream>
#include <string>

#include "nes_utils.h"
#include <aiko_includes.h>

namespace nes::test
{

    void NesTest::init()
    {
        std::ifstream file("C:/Users/j.iznardo/Documents/Aiko/examples/nes6502/assets/nestest.log");
        std::string str;
        while (std::getline(file, str))
        {
            LineCode code;

            code.pc = toWord(str.substr(0, 4));

            auto parameters = str.substr(6, 8);

            code.one = toByte(parameters.substr(0, 2));
            code.two = toByte(parameters.substr(2, 4));
            code.thr = toByte(parameters.substr(6, 8));

            auto one = toString(code.one);
            auto two = toString(code.two);
            auto thr = toString(code.thr);

            code.instruction = str.substr(16, 3);

            auto status = str.substr(48, 25);


            code.a = toByte(status.substr( 2, 2));
            code.x = toByte(status.substr( 7, 2));
            code.y = toByte(status.substr(12, 2));
            code.p = toByte(status.substr(17, 2));
            code.sp = toByte(status.substr(23, 2));

            lines.push_back(code);

        }
    }

    void NesTest::test(Word line, Word programCounter, Byte stackPointer, Byte A, Byte X, Byte Y, Byte P)
    {
        if (initialized == false)
        {
            init();
            initialized = true;
        }
        auto& test = lines[line];
        if (test.pc != programCounter)
        {
            aiko::Log::warning("ERROR :: PC :: EXPECTED : ", toString(test.pc), " -> RECIVED : ", toString(programCounter));
            int a = 0;
        }
        if (test.a != A)
        {
            aiko::Log::warning("ERROR :: A :: EXPECTED : ", toString(test.a), " -> RECIVED : ", toString(A));
            int a = 0;
        }
        if (test.x != X)
        {
            aiko::Log::warning("ERROR :: X :: EXPECTED : ", toString(test.x), " -> RECIVED : ", toString(X));
            int a = 0;
        }
        if (test.y != Y)
        {
            aiko::Log::warning("ERROR :: Y :: EXPECTED : ", toString(test.y), " -> RECIVED : ", toString(Y));
            int a = 0;
        }
        if (test.sp != stackPointer)
        {
            aiko::Log::warning("ERROR :: SP :: EXPECTED : ", toString(test.sp), " -> RECIVED : ", toString(stackPointer));
            int a = 0;
        }
        if (test.p != P)
        {
            aiko::Log::warning("ERROR :: P :: EXPECTED : ", toString(test.p), " -> RECIVED : ", toString(P));
            int a = 0;
        }
    }

}
