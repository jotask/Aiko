#include "nes_test_log.h"

#include <fstream>
#include <string>
#include <algorithm>
#include <random>
#include <functional>
#include <stack>

#include "nes/utils/nes_utils.h"
#include "nes/cpu/cpu.h"
#include "memory"

#include <aiko_includes.h>
#include "constants.h"

namespace nes::test
{

    void NesTest::init()
    {
        const std::string nes_log_path = global::getAssetPath("nestest.log");
        std::ifstream file(nes_log_path);
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

            auto status = str.substr(48, 47);

            code.a = toByte(status.substr( 2, 2));
            code.x = toByte(status.substr( 7, 2));
            code.y = toByte(status.substr(12, 2));
            code.p = toByte(status.substr(17, 2));
            code.sp = toByte(status.substr(23, 2));

            code.ppu_one = toByte(status.substr(30, 3));
            code.ppu_two = toByte(status.substr(34, 3));
            code.cycle = toWord(status.substr(34, 3));

            lines.push_back(code);

        }
    }

    void NesTest::test(Word line, OpCode op, Word programCounter, Byte stackPointer, Byte A, Byte X, Byte Y, Byte P)
    {
        if (initialized == false)
        {
            init();
            initialized = true;
        }
        if (line >= lines.size())
        {
            return;
        }
        auto& test = lines[line];
        bool success = true;

        // prev lines
        {
            auto& prev = lines[line - 1];
            auto toUpper = [&](std::string str) -> std::string
            {
                std::transform(str.begin(), str.end(), str.begin(), ::toupper);
                return str;
            };
            if (prev.instruction.compare( std::string(to_string(op.instruction))) == false )
            {
                aiko::Log::warning("ERROR :: INS :: EXPECTED : ", prev.instruction, " -> RECEIVED : ", std::string(to_string(op.instruction)));
                success = false;
            }
        }

        if (test.pc != programCounter)
        {
            aiko::Log::warning("ERROR :: PC :: EXPECTED : ", toString(test.pc), " -> RECEIVED : ", toString(programCounter));
            success = false;
        }
        if (test.a != A)
        {
            aiko::Log::warning("ERROR :: A :: EXPECTED : ", toString(test.a), " -> RECEIVED : ", toString(A));
            success = false;
        }
        if (test.x != X)
        {
            aiko::Log::warning("ERROR :: X :: EXPECTED : ", toString(test.x), " -> RECEIVED : ", toString(X));
            success = false;
        }
        if (test.y != Y)
        {
            aiko::Log::warning("ERROR :: Y :: EXPECTED : ", toString(test.y), " -> RECEIVED : ", toString(Y));
            success = false;
        }
        if (test.sp != stackPointer)
        {
            aiko::Log::warning("ERROR :: SP :: EXPECTED : ", toString(test.sp), " -> RECEIVED : ", toString(stackPointer));
            success = false;
        }
        if (test.p != P)
        {
            aiko::Log::warning("ERROR :: P :: EXPECTED : ", unsigned(test.p), " -> RECEIVED : ", unsigned(P));
            printStatusFlags("    EXPECTED:", test.p);
            printStatusFlags("    RECEIVED:", P);
            success = false;
        }
        // TODO test cycles aswell
        assert(success);

    }

}
