#include "online_test_manager.h"

#include <string>
#include <streambuf>
#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <random>
#include <mutex>
#include <thread>

#include <core/log.h>

#include <json/json.h>

namespace nes::test::online
{

    void TestManager::run()
    {

        const std::string GLOBAL_PATH = "C:/Users/Jose/Documents/Aiko/examples/nes6502/assets/tests/";

        auto parse = [&](std::string str)
            {
                Json::Reader reader;
                Json::Value root;
                reader.parse(str, root);

                for (Json::Value::ArrayIndex i = 0; i != root.size(); i++)
                {
                    Json::Value tmp = root[i];
                    Json::FastWriter fastWriter;
                    std::string output = fastWriter.write(tmp);
                    tests.push_back(OnlinesTest(output));
                }
    };

        auto readFile = [&](std::filesystem::path path)
            {
                std::ifstream t(path);
                std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                t.close();
                parse(str);
            };

        auto runByteOpCode = [&](Byte opCode)
        {
            tests.clear();

            auto fin = GLOBAL_PATH + toString(opCode).substr(2, 2) + ".json";
            readFile(fin);

            for (OnlinesTest& t : tests)
            {
                aiko::Log::info("RUN ", t.name);
                t.run();
                aiko::Log::info("   Test passed ", t.name);
            }

            int a = 0;
        };

#if false
        for (Byte i = 0x2a ; i <= 0xff ; i++)
        {
            runByteOpCode(i);
        }
#else

#if false
        runByteOpCode(0x01); // ORA IZX     -> OK
        runByteOpCode(0x05); // ORA ZP      -> OK
        runByteOpCode(0x06); // ASL ZP      -> OK
        runByteOpCode(0x09); // ORA IMM     -> OK
        runByteOpCode(0x0a); // ASL         -> OK
        runByteOpCode(0x0d); // ORA ABS     -> OK
        runByteOpCode(0x0e); // ASL ABS     -> OK
        runByteOpCode(0x10); // BPL REL     -> OK
        runByteOpCode(0x11); // ORA IZY     -> OK
        runByteOpCode(0x15); // ORA ZPX     -> OK
        runByteOpCode(0x16); // ASL ZPX     -> OK
        runByteOpCode(0x18); // CLC         -> OK
        runByteOpCode(0x19); // ORA ABY     -> OK
        runByteOpCode(0x1d); // ORA ABX     -> OK
        runByteOpCode(0x21); // AND IZX     -> OK
        runByteOpCode(0x25); // AND ZP      -> OK
        runByteOpCode(0x29); // AND IMM     -> OK
        runByteOpCode(0x2a); // ROL         -> OK
        runByteOpCode(0x2d); // AND ABS     -> OK
        runByteOpCode(0x31); // AND IZY     -> OK
        runByteOpCode(0x35); // AND ZPX     -> OK
        runByteOpCode(0x38); // SEC         -> OK
        runByteOpCode(0x39); // AND ABY     -> OK
        runByteOpCode(0x3d); // AND ABX     -> OK
        runByteOpCode(0x49); // EOR IMM     -> OK
        runByteOpCode(0x4a); // LSR         -> OK
        runByteOpCode(0x58); // CLI         -> OK
        runByteOpCode(0x69); // ADC IMM     -> OK
        runByteOpCode(0x6a); // ROR         -> OK
        runByteOpCode(0x70); // BVS REL     -> OK
        runByteOpCode(0x78); // SEI         -> OK
        runByteOpCode(0x88); // DEY         -> OK
        runByteOpCode(0x8a); // TXA         -> OK
        runByteOpCode(0x98); // TYA         -> OK
        runByteOpCode(0x9a); // TXS         -> OK
        runByteOpCode(0xa0); // LDY IMM     -> OK
        runByteOpCode(0xa1); // LDA IZX     -> OK
        runByteOpCode(0xa2); // LDX IMM     -> OK
        runByteOpCode(0xa5); // LDA ZP      -> OK
        runByteOpCode(0xa8); // TAY         -> OK
        runByteOpCode(0xa9); // LDA IMM     -> OK
        runByteOpCode(0xaa); // TAX         -> OK
        runByteOpCode(0xad); // LDA ABS     -> OK
        runByteOpCode(0xb1); // LDA IZY     -> OK
        runByteOpCode(0xb5); // LDA ZPX     -> OK
        runByteOpCode(0xb8); // CLV         -> OK
        runByteOpCode(0xb9); // LDA ABY     -> OK
        runByteOpCode(0xba); // TSX         -> OK
        runByteOpCode(0xbd); // LDA ABX     -> OK
        runByteOpCode(0xc0); // CPY IMM     -> OK
        runByteOpCode(0xc4); // CPY ZP      -> OK
        runByteOpCode(0xca); // DEX         -> OK
        runByteOpCode(0xcc); // CPY ABS     -> OK
        runByteOpCode(0xc8); // INY         -> OK
        runByteOpCode(0xc9); // CMP IMM     -> OK
        runByteOpCode(0xd8); // CLD         -> OK
        runByteOpCode(0xe0); // CPX IMM     -> OK
        runByteOpCode(0xe8); // INX         -> OK
        runByteOpCode(0xe9); // SBC IMM     -> OK
        runByteOpCode(0xea); // NOP         -> OK
        runByteOpCode(0xf8); // SED         -> OK
#else

        int z = 0;

        // runByteOpCode(0x00); // BRK
        runByteOpCode(0x06); // ASL ZP
        // runByteOpCode(0x08); // PHP

        runByteOpCode(0x0e); // ASL ABS

        runByteOpCode(0x16); // ASL ZPX
        runByteOpCode(0x1e); // ASL ABX

        runByteOpCode(0x28); // PLP
        runByteOpCode(0x40); // PHA
        runByteOpCode(0x40); // RTI

#endif
#endif

        int a = 0;

    }

}
