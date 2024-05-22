#include "online_test.h"

#include <json/json.h>
#include <tuple>
#include <map>

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
        cpu->waitForCycles = 0;
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

            struct CycleCpu
            {
                nes::Word address;
                nes::Byte value;
                std::string name;
            };

            for (size_t i = 0 ; i < cycles.size(); i++)
            {
                cpu->clock();
                auto getMap = [&]() -> std::map<nes::Word, CycleCpu>
                    {
                        std::map<nes::Word, CycleCpu> map;
                        for(auto& it : cycles)
                        {
                            nes::Word address = std::get<0>(it);
                            nes::Byte value = std::get<1>(it);
                            std::string name = std::get<2>(it);
                            if (map.find(address) == map.end())
                            {
                                map.insert({ address, { address, value, name } }); // not found
                            }
                            else
                            {
                                map[address] = { address, value, name }; // found
                            }
                        }
                        return map;
                    };
                auto map = getMap();
                const auto& cycle = cycles[i];
                for (size_t x = i; x < cpu->waitForCycles; x++)
                {
                    CycleCpu state = map[std::get<0>(cycle)];
                    nes::Byte memoryAddressValue = mem->read(state.address);
                    if (memoryAddressValue != state.value)
                    {
                        aiko::Log::error("Memory is not the expected. Cycle: " , unsigned(x), " Expected: ", unsigned(state.address), " -> ", unsigned(state.value), " Received: ", unsigned(cpu->program_counter), " -> ", unsigned(memoryAddressValue));
                        assert(false , "Memory is not the expected" );
                    }
                }
                i += cpu->waitForCycles;
                cpu->waitForCycles = 0;
            }
        }
        // 4. Check if final status it's the same as the final status
        {
            if (cpu->program_counter != final.pc)
            {
                aiko::Log::error("Program Counter not the same. Expected: ", unsigned(final.pc), " Received: ", unsigned(cpu->program_counter));
                assert(false);
            }
            if (cpu->stack_pointer != final.s)
            {
                aiko::Log::error("Stack Pointer not the same. Expected: ", unsigned(final.s), " Received: ", unsigned(cpu->stack_pointer));
                assert(false);
            }
            if (cpu->A != final.a)
            {
                aiko::Log::error("A not the same. Expected: ", unsigned(final.a), " Received: ", unsigned(cpu->A));
                assert(false);
            }
            if (cpu->X != final.x)
            {
                aiko::Log::error("X not the same. Expected: ", unsigned(final.x), " Received: ", unsigned(cpu->X));
                assert(false);
            }
            if (cpu->Y != final.y)
            {
                aiko::Log::error("Y not the same. Expected: ", unsigned(final.y), " Received: ", unsigned(cpu->Y));
                assert(false);
            }
            if (cpu->getP() != final.p)
            {
                aiko::Log::error("P not the same. Expected: ", unsigned(final.p), " Received: ", unsigned(cpu->getP()));

                auto printP = [&](Byte b, const char* prefix)
                    {
                        auto getFlag = [](Byte P, Cpu::StatusFlags p)
                            {
                                return ((P & p) > 0) ? 1 : 0;
                            };
                        aiko::Log::error(
                            prefix,
                            "StatusFlags::",
                            " C: ", unsigned(getFlag(b, Cpu::C)),
                            " Z: ", unsigned(getFlag(b, Cpu::Z)),
                            " I: ", unsigned(getFlag(b, Cpu::I)),
                            " D: ", unsigned(getFlag(b, Cpu::D)),
                            " B: ", unsigned(getFlag(b, Cpu::B)),
                            " U: ", unsigned(getFlag(b, Cpu::U)),
                            " V: ", unsigned(getFlag(b, Cpu::V)),
                            " N: ", unsigned(getFlag(b, Cpu::N))
                        );
                    };

                printP(final.p, "Expected: ");
                printP(cpu->getP(), "Received: ");
                assert(false);
            }
            for (auto& r : final.ram)
            {
                nes::Word address = std::get<0>(r);
                nes::Byte value = std::get<1>(r);
                nes::Byte readed = mem->read(address);
                if (readed != value)
                {
                    aiko::Log::error("Memory is not the expected. Address: " , unsigned(address), " Expected: ", unsigned(value), " Readed : ", unsigned(readed));
                    assert(false);
                }
            }
        }
    }

}
