#include "nes6502.h"

#include <chrono>
#include <sstream>
#include <cassert>

#include "shared/math.h"
#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "systems/render_system.h"
#include "core/utils.h"
#include "types/inputs.h"
#include "models/camera.h"
#include "shared/math.h"
#include "core/log.h"

namespace nes
{
    nes6502::nes6502()
        : runClock(true)
        , nOfcycles(0)
    {
        init();
    }

    nes6502::~nes6502()
    {
        stop();
    }

    void nes6502::init()
    {

        reset();

        bus.m_nes = this;

        bus.addMicroprocesor(&cartridge);
        bus.addMicroprocesor(&cpu);
        bus.addMicroprocesor(&memory);
        bus.addMicroprocesor(&ppu);

    }

    void nes6502::start()
    {
        runClock = true;
        clock = std::thread(&nes6502::onCycle, this);
    }

    void nes6502::stop()
    {
        runClock = false;
        if (std::thread::id() != clock.get_id())
        {
            clock.join();
        }
    }

    void nes6502::reset()
    {
        bus.reset();
        cartridge.reset();
        cpu.reset();
        memory.reset();
        ppu.reset();
    }

    void nes6502::onCycle()
    {
        using namespace std::literals;
        using clock_type = std::chrono::high_resolution_clock;

        constexpr auto wait_time = 1ms;// 1000ms;

        auto when_started = clock_type::now();
        auto target_time = when_started + wait_time;
        while(runClock)
        {
            bus.clock();
            nOfcycles++;
            std::this_thread::sleep_until(target_time);
            target_time += wait_time;
            aiko::Log::info("[6502] clock tick");
        }

    }

    void nes6502::insertCartridge(const char* file)
    {
        cartridge.load(file);
    }

    std::size_t nes6502::getNofCycles() const
    {
        return nOfcycles;;
    }

    nes::Bus* nes6502::getBus()
    {
        return &bus;
    }

}
