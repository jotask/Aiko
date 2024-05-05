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

namespace test::online
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
            state.p = root["p"].asInt();

            Json::Value var_ram = root["ram"];
            for (Json::Value::ArrayIndex i = 0; i != var_ram.size(); i++)
            {
                nes::Word one = var_ram[i][0].asInt();
                nes::Byte two = var_ram[i][1].asInt();
                state.ram.push_back({ one, two });
            }

        };

        set_CpuState(root["initial"], initial);
        set_CpuState(root["final"], final);

        {
            Json::Value var_cycles = root["cycles"];
            for (Json::Value::ArrayIndex i = 0; i != var_cycles.size(); i++)
            {
                nes::Word one = var_cycles[i][0].asInt();
                nes::Byte two = var_cycles[i][1].asInt();
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

            // Extract individual status flags
            cpu->C = (initial.p >> 0) & 0x01; // Carry flag (bit 0)
            cpu->Z = (initial.p >> 1) & 0x01; // Zero flag (bit 1)
            cpu->I = (initial.p >> 2) & 0x01; // Interrupt disable flag (bit 2)
            cpu->D = (initial.p >> 3) & 0x01; // Decimal mode flag (bit 3)
            cpu->B = (initial.p >> 4) & 0x01; // Break command flag (bit 4)
            // Bit 5 is unused and is typically set to 1
            cpu->V = (initial.p >> 6) & 0x01; // Overflow flag (bit 6)
            cpu->N = (initial.p >> 7) & 0x01; // Negative flag (bit 7)

            for ( auto& r : initial.ram )
            {
                nes::Word address = std::get<0>(r);
                nes::Byte value = std::get<1>(r);
                mem->write(address, value);
            }

        }
        // 3. Execute run
        {
            constexpr nes::Word MAX_ITERATIONS = 0xFFFF;
            nes::Word iteration = 0x0000;
            while (cpu->program_counter != final.pc)
            {
                // Execute next instruction
                cpu->clock();
                iteration++;
                assert(iteration < MAX_ITERATIONS, "Max number of iterations reached");
            }
        }
        // 4. Check if final status it's the same as the final status
        {
            assert(cpu->program_counter == final.pc, "program_counter not the same");
            assert(cpu->stack_pointer == final.s, "stack_pointer not the same");
            assert(cpu->A == final.a, "A not the same");
            assert(cpu->X == final.x, "X not the same");
            assert(cpu->Y == final.y, "Y not the same");
            // assert(cpu->getP() == final.p, "P not the same");
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
