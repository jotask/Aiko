#include "nes_test_log.h"

#include <fstream>
#include <string>
#include <algorithm>
#include <random>
#include <functional>
#include <stack>

#include "nes_utils.h"
#include "cpu.h"
#include "memory"

#include <aiko_includes.h>

namespace nes::test
{

    void NesTest::init()
    {
        const std::string nes_log_path = AssetPath + "nestest.log";
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

            auto status = str.substr(48, 25);


            code.a = toByte(status.substr( 2, 2));
            code.x = toByte(status.substr( 7, 2));
            code.y = toByte(status.substr(12, 2));
            code.p = toByte(status.substr(17, 2));
            code.sp = toByte(status.substr(23, 2));

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
                aiko::Log::warning("ERROR :: INS :: EXPECTED : ", prev.instruction, " -> RECIVED : ", std::string(to_string(op.instruction)));
                success = false;
            }
        }

        if (test.pc != programCounter)
        {
            aiko::Log::warning("ERROR :: PC :: EXPECTED : ", toString(test.pc), " -> RECIVED : ", toString(programCounter));
            success = false;
        }
        if (test.a != A)
        {
            aiko::Log::warning("ERROR :: A :: EXPECTED : ", toString(test.a), " -> RECIVED : ", toString(A));
            success = false;
        }
        if (test.x != X)
        {
            aiko::Log::warning("ERROR :: X :: EXPECTED : ", toString(test.x), " -> RECIVED : ", toString(X));
            success = false;
        }
        if (test.y != Y)
        {
            aiko::Log::warning("ERROR :: Y :: EXPECTED : ", toString(test.y), " -> RECIVED : ", toString(Y));
            success = false;
        }
        if (test.sp != stackPointer)
        {
            aiko::Log::warning("ERROR :: SP :: EXPECTED : ", toString(test.sp), " -> RECIVED : ", toString(stackPointer));
            success = false;
        }
        if (test.p != P)
        {
            aiko::Log::warning("ERROR :: P :: EXPECTED : ", unsigned(test.p), " -> RECIVED : ", unsigned(P));
            printStatusFlags("    EXPECTED:", test.p);
            printStatusFlags("    RECEIVED:", P);
            success = false;
        }
        // TODO test cycles aswell
        assert(success);

    }

    void NesTest::testStack(nes::Cpu* cpu, nes::Memory* mem)
    {

        std::random_device dev;
        std::mt19937 rng(dev());
        auto tmp = sizeof(Byte);
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, 0xff); // distribution in range [1, 6]
        auto shit = dist(rng);

        auto rnd = [&]() -> Byte { return dist(rng); };

        std::stack<Byte> stack;
        std::stack<Word> stackW;

        auto popB = [&]() -> Byte
            {
                Byte res1 = stack.top();
                stack.pop();
                Byte res2 = cpu->popStack();

                if (res1 != res2)
                {
                    aiko::Log::error("Stack Test Failed");
                    int a = 0;
                }

                return res1;
            };

        auto popW = [&]() -> Word
            {
                Byte high = stack.top();
                stack.pop();
                Byte low = stack.top();
                stack.pop();

                Word top = stackW.top();
                stackW.pop();
                auto str0 = toString(top);

                Word res1 = toWord(high, low);
                auto str1 = toString(res1);

                Word res2 = cpu->popWordStack();
                auto str2 = toString(res2);

                if (top != res1 || top != res2 || res1 != res2)
                {
                    aiko::Log::error("Stack Test Failed");
                    int a = 0;
                }

                return res1;
            };


        auto pushB = [&](Byte val)
            {
                stack.push(val);
                cpu->pushStack(val);
            };

        auto pushW = [&](Byte high, Byte low)
            {
                stack.push(low);
                stack.push(high);
                Word tmp = toWord(high, low);
                auto shit = toString(tmp);
                stackW.push(tmp);
                cpu->pushWordStack(tmp);
                // cpu->pushStack(low);
                // cpu->pushStack(high);
                int a = 0;
            };

#if true

        constexpr Byte times = 1;

        for (Byte i = 0; i < times; i++)
        {
            pushB(rnd());
        }
        for (Byte i = 0; i < times; i++)
        {
            popB();
        }

        for (Byte i = 0; i < times; i++)
        {
            // pushW(rnd(), rnd());
            pushW(0xde, 0xad);
        }
        for (Byte i = 0; i < times; i++)
        {
            popW();
        }

        int a = 0;

#endif
    }


}
