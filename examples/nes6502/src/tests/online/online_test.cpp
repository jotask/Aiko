#include "online_test.h"

#include <json/json.h>

#include "bus.h"
#include "cpu.h"
#include "memory.h"
#include "cartridge.h"
#include "tests/test_type.h"
#include "nes_utils.h"
#include "nes6502.h"

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
        // TODO
        
        // 1. Create the nest instance
        nes::nes6502 nes;
        nes::Bus* bus = nes.getBus();
        nes::Cpu* cpu = bus->getMicroprocesor<nes::Cpu>();
        {
            // 2. Set the nest state tot he initial data
        }
        {
            // 3. Execute run
        }
        {
            // 4. Check if final status it's the same as the final status
        }
    }

}
