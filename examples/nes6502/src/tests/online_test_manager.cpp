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

#include <aiko_includes.h>

#include <json/json.h>

namespace nes::test::online
{

    void TestManager::run()
    {

        const std::string GLOBAL_PATH = AssetPath + "tests/";

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
        for (Byte i = 0x00 ; i <= 0xff ; i++)
        {
            runByteOpCode(i);
        }
#else

#if false
        runByteOpCode(0x01); // ORA IZX     -> OK
        runByteOpCode(0x05); // ORA ZP      -> OK
        runByteOpCode(0x06); // ASL ZP      -> OK
        runByteOpCode(0x08); // PHP
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
        runByteOpCode(0x1e); // ASL ABX
        runByteOpCode(0x20); // JSR ABS
        runByteOpCode(0x21); // AND IZX     -> OK
        runByteOpCode(0x24); // BIT ZP
        runByteOpCode(0x25); // AND ZP      -> OK
        runByteOpCode(0x26); // ROL ZP
        runByteOpCode(0x28); // PLP
        runByteOpCode(0x29); // AND IMM     -> OK
        runByteOpCode(0x2a); // ROL         -> OK
        runByteOpCode(0x2c); // BIT ABS
        runByteOpCode(0x2d); // AND ABS     -> OK
        runByteOpCode(0x2e); // ROL ABS
        runByteOpCode(0x30); // BMI REL
        runByteOpCode(0x31); // AND IZY     -> OK
        runByteOpCode(0x35); // AND ZPX     -> OK
        runByteOpCode(0x36); // ROL ZPX
        runByteOpCode(0x38); // SEC         -> OK
        runByteOpCode(0x39); // AND ABY     -> OK
        runByteOpCode(0x3d); // AND ABX     -> OK
        runByteOpCode(0x3e); // ROL ABX
        runByteOpCode(0x40); // RTI
        runByteOpCode(0x41); // EOR IZX
        runByteOpCode(0x45); // EOR ZP
        runByteOpCode(0x46); // LSR ZP
        runByteOpCode(0x48); // PHA
        runByteOpCode(0x49); // EOR IMM     -> OK
        runByteOpCode(0x4a); // LSR         -> OK
        runByteOpCode(0x4c); // JMP ABS
        runByteOpCode(0x4d); // EOR ABS
        runByteOpCode(0x4e); // LSR ABS
        runByteOpCode(0x58); // CLI         -> OK
        runByteOpCode(0x50); // BVC REL
        runByteOpCode(0x51); // EOR IZY
        runByteOpCode(0x55); // EOR ZPX
        runByteOpCode(0x56); // LSR APZ
        runByteOpCode(0x59); // EOR ABY
        runByteOpCode(0x5d); // EOR ABX
        runByteOpCode(0x5e); // LSR ABX
        runByteOpCode(0x60); // RTS
        runByteOpCode(0x61); // ADC IZX
        runByteOpCode(0x65); // ADC ZP
        runByteOpCode(0x66); // ROR ZP
        runByteOpCode(0x68); // PLA
        runByteOpCode(0x69); // ADC IMM     -> OK
        runByteOpCode(0x6a); // ROR         -> OK
        runByteOpCode(0x6c); // JMP IND
        runByteOpCode(0x6d); // ADC ABS
        runByteOpCode(0x6e); // ROR ABS
        runByteOpCode(0x70); // BVS REL     -> OK
        runByteOpCode(0x71); // ADC IZY
        runByteOpCode(0x75); // ADC ZPX
        runByteOpCode(0x76); // ROR ZPX
        runByteOpCode(0x78); // SEI         -> OK
        runByteOpCode(0x79); // ADC ABY
        runByteOpCode(0x7d); // ADC ABX
        runByteOpCode(0x7e); // ROR ABX
        runByteOpCode(0x81); // STA IZX
        runByteOpCode(0x84); // STY IZY
        runByteOpCode(0x85); // STA ZP
        runByteOpCode(0x86); // STX ZP
        runByteOpCode(0x88); // DEY         -> OK
        runByteOpCode(0x8a); // TXA         -> OK
        runByteOpCode(0x8c); // STY ABS
        runByteOpCode(0x8d); // STA ABS
        runByteOpCode(0x8e); // STX ABS
        runByteOpCode(0x90); // BCC REL
        runByteOpCode(0x91); // STA IZY
        runByteOpCode(0x94); // STY ZPX
        runByteOpCode(0x95); // STA ZPX
        runByteOpCode(0x96); // STX ZPY
        runByteOpCode(0x98); // TYA         -> OK
        runByteOpCode(0x99); // STA ABY
        runByteOpCode(0x9a); // TXS         -> OK
        runByteOpCode(0x9d); // STA ABX
        runByteOpCode(0xa0); // LDY IMM     -> OK
        runByteOpCode(0xa1); // LDA IZX     -> OK
        runByteOpCode(0xa2); // LDX IMM     -> OK
        runByteOpCode(0xa4); // LDY ZP
        runByteOpCode(0xa5); // LDA ZP      -> OK
        runByteOpCode(0xa6); // LDX ZP
        runByteOpCode(0xa8); // TAY         -> OK
        runByteOpCode(0xa9); // LDA IMM     -> OK
        runByteOpCode(0xaa); // TAX         -> OK
        runByteOpCode(0xac); // LDY ABS
        runByteOpCode(0xad); // LDA ABS     -> OK
        runByteOpCode(0xae); // LDX ABS
        runByteOpCode(0xb0); // BCS REL
        runByteOpCode(0xb1); // LDA IZY     -> OK
        runByteOpCode(0xb4); // LDY ZPX
        runByteOpCode(0xb5); // LDA ZPX     -> OK
        runByteOpCode(0xb6); // LDX ZPY
        runByteOpCode(0xb8); // CLV         -> OK
        runByteOpCode(0xb9); // LDA ABY     -> OK
        runByteOpCode(0xba); // TSX         -> OK
        runByteOpCode(0xbc); // LDY ABX
        runByteOpCode(0xbd); // LDA ABX     -> OK
        runByteOpCode(0xbe); // LDX ABY
        runByteOpCode(0xc0); // CPY IMM     -> OK
        runByteOpCode(0xc1); // CPM IZX
        runByteOpCode(0xc4); // CPY ZP      -> OK
        runByteOpCode(0xc5); // CMP ZP
        runByteOpCode(0xc6); // DEC CP
        runByteOpCode(0xc8); // INY         -> OK
        runByteOpCode(0xc9); // CMP IMM     -> OK
        runByteOpCode(0xca); // DEX         -> OK
        runByteOpCode(0xcc); // CPY ABS     -> OK
        runByteOpCode(0xcd); // CMP ABS
        runByteOpCode(0xce); // DEC ABS
        runByteOpCode(0xd0); // BNE REL
        runByteOpCode(0xd1); // CMP IZY
        runByteOpCode(0xd5); // CMP ZPX
        runByteOpCode(0xd6); // DEC ZPX
        runByteOpCode(0xd8); // CLD         -> OK
        runByteOpCode(0xd9); // CMP ABY
        runByteOpCode(0xdd); // CMP ABX
        runByteOpCode(0xde); // DEC ABX
        runByteOpCode(0xe0); // CPX IMM     -> OK
        runByteOpCode(0xe1); // SBC IZX
        runByteOpCode(0xe4); // CPX ZP
        runByteOpCode(0xe5); // SBC ZP
        runByteOpCode(0xe6); // INC ZP
        runByteOpCode(0xe8); // INX         -> OK
        runByteOpCode(0xe9); // SBC IMM     -> OK
        runByteOpCode(0xea); // NOP         -> OK
        runByteOpCode(0xec); // CPX ABS
        runByteOpCode(0xed); // SBC ABS
        runByteOpCode(0xee); // INC ABS
        runByteOpCode(0xf0); // BEQ REL
        runByteOpCode(0xf1); // SBC IZY
        runByteOpCode(0xf5); // SBC ZPX
        runByteOpCode(0xf6); // INC ZPX
        runByteOpCode(0xf8); // SED         -> OK
        runByteOpCode(0xf9); // SBC ABY
        runByteOpCode(0xfd); // SBC ABX
        runByteOpCode(0xfe); // INC ABX
#else

        runByteOpCode(0x00); // BRK

#endif
#endif

        int a = 0;

    }

}
