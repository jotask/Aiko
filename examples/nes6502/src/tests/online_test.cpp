#include "online_test.h"

#include <json/json.h>
#include <tuple>

#include "bus.h"
#include "cpu.h"
#include "memory.h"
#include "cartridge.h"
#include "tests/test_type.h"
#include "nes_utils.h"
#include "nes6502.h"

#include "tests/test_type.h"

namespace nes::test::online
{
    OnlinesTest::OnlinesTest(std::string test)
    {
        Json::Value root;
        Json::Reader reader;
        reader.parse(test, root);

        name = root["name"].asString();

        auto set_CpuState = [](Json::Value root, CpuState& state)
        {

            state.pc = root["pc"].asInt();
            state.s = root["s"].asInt();
            state.a = root["a"].asInt();
            state.x = root["x"].asInt();
            state.y = root["y"].asInt();
            state.p = root["p"].asInt();

            Json::Value var_ram = root["ram"];
            for (Json::Value::ArrayIndex i = 0; i != var_ram.size(); i++)
            {
                auto entry = var_ram[i];
                const nes::Word one = entry[0].asUInt();
                const nes::Byte two = entry[1].asUInt();
                state.ram.push_back({ one, two });
            }

        };

        set_CpuState(root["initial"], initial);
        set_CpuState(root["final"], final);

        {
            Json::Value var_cycles = root["cycles"];
            for (Json::Value::ArrayIndex i = 0; i != var_cycles.size(); i++)
            {
                const nes::Word one = var_cycles[i][0].asInt();
                const nes::Byte two = var_cycles[i][1].asInt();
                std::string thre = var_cycles[i][2].asString();
                cycles.push_back({ one, two, thre });
            }
        }

    }

    void OnlinesTest::run()
    {
        // 1. Create the nest instance
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        nes::Memory* mem = bus->getMicroprocesor<nes::Memory>();
        // 2. Set the cpu state to the initial data
        {
            cpu->program_counter = initial.pc;
            cpu->stack_pointer = initial.s;
            cpu->A = initial.a;
            cpu->X = initial.x;
            cpu->Y = initial.y;

            cpu->setP(initial.p);

            for ( auto& r : initial.ram )
            {
                const nes::Word address = std::get<0>(r);
                const nes::Byte value = std::get<1>(r);
                mem->write(address, value);
            }

        }
        // 3. Execute run
        {
            for (size_t i = 0 ; i < cycles.size(); i++)
            {
                cpu->clock();
                auto& cycle = cycles[i];
                nes::Word address = std::get<0>(cycle);
                nes::Word value = std::get<1>(cycle);
                std::string name = std::get<2>(cycle);
                cpu->waitForCycles = 0;
                nes::Byte memoryAddressValue = mem->read(address);
                assert(memoryAddressValue == value, "Memory is not the expected" );
            }
        }
        // 4. Check if final status it's the same as the final status
        {
            assert(cpu->program_counter == final.pc, "program_counter not the same");
            assert(cpu->stack_pointer == final.s, "stack_pointer not the same");
            assert(cpu->A == final.a, "A not the same");
            assert(cpu->X == final.x, "X not the same");
            assert(cpu->Y == final.y, "Y not the same");
            assert(cpu->getP() == final.p, "P not the same");
            for (auto& r : final.ram)
            {
                nes::Word address = std::get<0>(r);
                nes::Byte value = std::get<1>(r);
                nes::Byte readed = mem->read(address);
                assert(readed == value, "Memory not the same");
            }
        }
    }

}
