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

    }

    nes6502::~nes6502()
    {
        runClock = false;
        clock.join();
    }

    void nes6502::init()
    {
        Application::init();

        reset();

        bus.m_nes = this;

        bus.addMicroprocesor(&cartridge);
        bus.addMicroprocesor(&cpu);
        bus.addMicroprocesor(&memory);
        bus.addMicroprocesor(&ppu);

        cartridge.load("C:/Users/j.iznardo/Documents/Aiko/examples/nes6502/tests/6502_functional_test.bin");

        runClock = true;
        clock = std::thread(&nes6502::onCycle, this);

    }

    void nes6502::update()
    {
    }

    void nes6502::render()
    {
        static std::stringstream fmt;
        constexpr float space = 15.0f;
        float y = 0.0f;
        drawText("nes6502", 0.0f, space * y++);
        fmt.str("");    fmt << "Cycles: " << nOfcycles;
        drawText( fmt.str(), 0.0f, space * y++);
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

        constexpr auto wait_time = 1000ms;

        auto when_started = clock_type::now();
        auto target_time = when_started + wait_time;
        while(runClock)
        {
            cpu.clock();
            nOfcycles++;
            std::this_thread::sleep_until(target_time);
            target_time += wait_time;
            aiko::Log::info("[6502] clock tick");
        }

    }

}
